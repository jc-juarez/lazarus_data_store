// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Main
// 'lazarus_data_store.cc'
// Author: jcjuarez
// Description:
//      Lazarus data store root object. 
// ****************************************************

#include <spdlog/spdlog.h>
#include "../common/aliases.hh"
#include "lazarus_data_store.hh"
#include "../network/server/server.hh"
#include "../common/args_validations.hh"
#include "../storage/io/data_partition.hh"
#include "../storage/io/read_io_executor.hh"
#include <spdlog/sinks/rotating_file_sink.h>
#include "../storage/cache/cache_accessor.hh"
#include "../storage/index/container_index.hh"
#include "../storage/cache/frontline_cache.hh"
#include "../storage/io/read_io_dispatcher.hh"
#include "../storage/index/container_loader.hh"
#include "../storage/io/collocation_resolver.hh"
#include "../storage/io/data_partition_provider.hh"
#include "../storage/io/threading_context_provider.hh"
#include "../storage/management/object_management_service.hh"
#include "../storage/management/container_management_service.hh"
#include "../storage/management/container_operation_serializer.hh"

namespace lazarus
{

lazarus_data_store::lazarus_data_store(
    const boost::uuids::uuid session_id,
    std::unique_ptr<storage::data_partition> containers_metadata_partition,
    std::unique_ptr<storage::collocation_resolver> collocation_resolver,
    std::unique_ptr<storage::data_partition_provider> data_partition_provider,
    std::unique_ptr<storage::threading_context_provider> threading_context_provider,
    std::unique_ptr<network::server> server,
    std::unique_ptr<storage::container_management_service> container_management_service,
    std::unique_ptr<storage::object_management_service> object_management_service,
    std::unique_ptr<storage::garbage_collector> garbage_collector,
    std::unique_ptr<storage::container_index> container_index,
    std::unique_ptr<storage::io_dispatcher_interface> write_io_task_dispatcher,
    std::unique_ptr<storage::io_dispatcher_interface> read_io_task_dispatcher,
    std::unique_ptr<storage::frontline_cache> frontline_cache,
    std::unique_ptr<storage::read_io_executor> object_io_executor,
    std::unique_ptr<storage::cache_accessor> cache_accessor,
    std::unique_ptr<storage::container_loader> container_loader)
    : session_id_{session_id}
    , containers_metadata_partition_{std::move(containers_metadata_partition)}
    , collocation_resolver_{std::move(collocation_resolver)}
    , data_partition_provider_{std::move(data_partition_provider)}
    , threading_context_provider_{std::move(threading_context_provider)}
    , server_{std::move(server)}
    , container_management_service_{std::move(container_management_service)}
    , object_management_service_{std::move(object_management_service)}
    , garbage_collector_{std::move(garbage_collector)}
    , container_index_{std::move(container_index)}
    , write_io_task_dispatcher_{std::move(write_io_task_dispatcher)}
    , read_io_task_dispatcher_{std::move(read_io_task_dispatcher)}
    , frontline_cache_{std::move(frontline_cache)}
    , object_io_executor_{std::move(object_io_executor)}
    , cache_accessor_{std::move(cache_accessor)}
    , container_loader_{std::move(container_loader)}
{}

status::status_code
lazarus_data_store::start_data_store()
{
    using references_mapping = std::unordered_map<std::string, storage::storage_engine_reference_handle*>;

    //
    // Before starting the server, boot the container
    // metadata partition to load the persistent metadata state.
    //
    references_mapping metadata_partition_references;
    status::status_code status = boot_data_partition(
        *containers_metadata_partition_,
        metadata_partition_references);

    if (status::failed(status))
    {
        spdlog::critical("Failed to boot container metadata partition during the system startup. "
            "Status={:#x}.",
            status);

        return status;
    }

    //
    // Boot up all structured data partitions which hold the master data.
    // This will make sure the underlying storage engines are ready for core data access.
    //
    auto boot_result = boot_structured_data_partitions();

    if (!boot_result)
    {
        spdlog::critical("Failed to boot structured data partitions during the system startup. "
            "Status={:#x}.",
            boot_result.error());

        return boot_result.error();
    }

    //
    // Perform an integrity validation on the containers found inside the structured
    // data partitions. Startup should be stopped on inconsistencies or corruption.
    //
    storage::container_registry structured_partitions_registry = boot_result.value();
    status = structured_partitions_registry.execute_integrity_validation();

    if (status::failed(status))
    {
        spdlog::critical("Integrity validation for the structured data partitions containers failed. "
            "Status={:#x}.",
            status);

        return status;
    }

    //
    // Populate the in-memory container index with the
    // references obtained when the data partitions were booted.
    // This will ensure the filesystem and metadata state are in agreement.
    //
    status = container_loader_->load_container_index(
        metadata_partition_references,
        structured_partitions_registry);

    if (status::failed(status))
    {
        spdlog::critical("Failed to load and populate the container index during the system startup. "
            "Status={:#x}.",
            status);

        return status;
    }

    //
    // Start the system garbage collector.
    // This needs to be started after all initial orphaned
    // object containers are discovered for proper initial cleanup.
    //
    garbage_collector_->start();

    //
    // Start the server for handling incoming data requests.
    // This will block the main thread.
    //
    server_->start();

    return status::success;
}

std::expected<
    storage::container_registry,
    status::status_code>
lazarus_data_store::boot_structured_data_partitions()
{
    //
    // Keep track of all container names and their respective
    // engine references present on the structured data partitions:
    // ----------------------------------------------
    // | ContainerX | ContainerY | ContainerZ | ... |
    // ----------------------------------------------
    // | 0x11223344 | 0x22334455 | 0x33445566 | ... |
    // | 0x44556677 | 0x55667788 | 0x66778899 | ... |
    // |    ...     |     ...    |     ...    | ... |
    // ----------------------------------------------
    //
    storage::container_registry container_registry;

    const std::span<storage::data_partition> data_partitions =
        data_partition_provider_->get_all_partitions();

    for (auto& data_partition : data_partitions)
    {
        std::unordered_map<std::string, storage::storage_engine_reference_handle*> structured_partitions_references;

        status::status_code status = boot_data_partition(
            data_partition,
            structured_partitions_references);

        if (status::failed(status))
        {
            spdlog::critical("Failed to boot structured data partition on CollocationIndex={}. "
                "Status={:#x}.",
                data_partition.get_collocation_index(),
                status);

            return std::unexpected(status);
        }

        //
        // Every registration into the registry will correspond to the respective collocation.
        //
        for (auto& reference_entry : structured_partitions_references)
        {
            container_registry.register_container_reference(
                reference_entry.first,
                reference_entry.second);
        }
    }

    return container_registry;
}

status::status_code
lazarus_data_store::boot_data_partition(
    storage::data_partition& data_partition,
    std::unordered_map<std::string, storage::storage_engine_reference_handle*>& references_mapping)
{
    //
    // Fetch all persistent object containers from the filesystem.
    // These are needed for starting the storage engine so that it can
    // later associate an object container name to its respective column family reference.
    // This is a static invocation being executed before the storage engine is started.
    //
    std::vector<std::string> containers_names;
    status::status_code status = data_partition.fetch_containers_from_disk(
        containers_names);

    if (status::failed(status))
    {
        spdlog::critical("Failed to fetch data partition object containers from disk during the system startup. "
            "Status={:#x}.",
            status);

        return status;
    }

    //
    // Boot the partition. On success, it will associate the object
    // containers names to their respective column family reference.
    //
    status = data_partition.boot(
        containers_names,
        references_mapping);

    if (status::failed(status))
    {
        spdlog::critical("Failed to boot data partition during the system startup. "
            "Status={:#x}.",
            status);

        return status;
    }

    return status::success;
}

} // namespace lazarus.