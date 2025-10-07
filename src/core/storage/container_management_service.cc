// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_management_service.cc'
// Author: jcjuarez
// Description:
//      Management service for object container
//      operations.
// ****************************************************

#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "garbage_collector.hh"
#include "container_index.hh"
#include "../common/request_validations.hh"
#include "container_management_service.hh"
#include "container_operation_serializer.hh"
#include "container_persistent_interface.pb.h"

namespace lazarus
{
namespace storage
{

container_management_service::container_management_service(
    const storage_configuration& storage_configuration,
    std::shared_ptr<storage_engine_interface> storage_engine_handle,
    std::shared_ptr<container_index> container_index_handle,
    std::unique_ptr<container_operation_serializer> container_operation_serializer_handle)
    : storage_configuration_{storage_configuration},
      storage_engine_(std::move(storage_engine_handle)),
      container_index_{std::move(container_index_handle)},
      container_operation_serializer_{std::move(container_operation_serializer_handle)}
{}

status::status_code
container_management_service::populate_container_index(
    std::unordered_map<std::string, storage_engine_reference_handle*>* storage_engine_references_mapping)
{
    //
    // Keep track of the internal metadata object containers.
    // The only case when it is valid for them to be absent is when the column
    // family references mapping only has 1 entry (kDefaultColumnFamilyName), which
    // implies that the data store is starting up for the first time.
    //
    if (storage_engine_references_mapping->size() == 1u)
    {
        //
        // This means this is the first ever startup for the data store,
        // or that it simply does not need a previous metadata state for working.
        // Create all required internal metadata root object containers.
        //
        create_internal_metadata_column_families(storage_engine_references_mapping);
    }

    //
    // This forcefully means that this is not the first time data store
    // startup. Ensure that all internal metadata object containers are present; if not,
    // this is a critical error as the data store cannot function without them.
    //
    if (storage_engine_references_mapping->find(container_index::containers_internal_metadata_name) ==
        storage_engine_references_mapping->end())
    {
        //
        // This is a critical error as one or some of the core internal
        // metadata column families were not found. Fail the system startup.
        //
        spdlog::critical("Failed to find find the storage engine reference for the '{}' internal metadata.",
                         container_index::containers_internal_metadata_name);

        return status::containers_internal_metadata_lookup_failed;
    }

    storage_engine_reference_handle* containers_internal_metadata_storage_engine_reference =
        storage_engine_references_mapping->at(container_index::containers_internal_metadata_name);

    //
    // Finally, get all known object containers to the system
    // from the storage engine and populate the rest of the object container index.
    //
    std::unordered_map<std::string, byte_stream> objects;
    status::status_code status = storage_engine_->get_all_objects_from_container(
        containers_internal_metadata_storage_engine_reference,
        &objects);

    if (status::failed(status))
    {
        spdlog::critical("Failed to get all object containers from the object containers internal metadata. "
            "Status={:#x}.",
            status);

        return status;
    }

    for (const auto& storage_engine_references_pair : *storage_engine_references_mapping)
    {
        std::string container_name = storage_engine_references_pair.first;
        storage_engine_reference_handle* container_storage_engine_reference = storage_engine_references_pair.second;

        if (objects.find(container_name) != objects.end())
        {
            //
            // This implies this is a known object container to the persistent metadata.
            //
            byte_stream& object_data = objects.at(container_name);
            schemas::container_persistent_interface container_persistent_metadata;
            const bool is_parsing_successful = container_persistent_metadata.ParseFromString(object_data);

            if (!is_parsing_successful)
            {
                spdlog::critical("Failed to parse an object container metadata on startup. "
                    "ObjectContainerName={}.",
                    container_name);

                return status::parsing_failed;
            }

            status = container_index_->insert_container(
                container_storage_engine_reference,
                container_persistent_metadata);

            if (status::failed(status))
            {
                spdlog::critical("Failed to insert container into the container index. "
                    "ContainerName={}, "
                    "Status={:#x}.",
                    container_persistent_metadata.name().c_str(),
                    status);

                return status;
            }

            const std::shared_ptr<container> container =
                container_index_->get_container(container_name);

            spdlog::info("Found object container on startup. Indexing into the object containers metadata table. "
                "ObjectContainerMetadata={}.",
                container->to_string());
        }
        else
        {
            //
            // This implies this is not a known object container to the persistent metadata.
            // These could be internal metadata or orphaned object containers.
            //
            const schemas::container_persistent_interface container_persistent_metadata =
                container::create_container_persistent_metadata(container_name.c_str());
            status = container_index_->insert_container(
                container_storage_engine_reference,
                container_persistent_metadata);

            if (status::failed(status))
            {
                spdlog::critical("Failed to insert container into the container index. "
                    "ContainerName={}, "
                    "Status={:#x}.",
                    container_persistent_metadata.name().c_str(),
                    status);

                return status;
            }

            const bool is_orphaned_container =
                !container_index::is_internal_metadata_container(container_name);

            if (is_orphaned_container)
            {
                //
                // If this is not part of the internal metadata, it means that this is an
                // orphaned object container; mark it as deleted for the garbage collector to clean it up later.
                //
                spdlog::warn("Found orphaned object container on startup to be cleaned up by the garbage collector. "
                    "ObjectContainerName={}.",
                    container_name.c_str());
                
                std::shared_ptr<container> container =
                    container_index_->get_container(container_name);

                if (container == nullptr)
                {
                    spdlog::critical("Failed to mark object container as deleted on startup. "
                        "ObjectContainerName={}.",
                        container_name.c_str());

                    return status;
                }

                container->mark_as_deleted();
            }
        }
    }

    return status::success;
}

status::status_code
container_management_service::create_internal_metadata_column_families(
    std::unordered_map<std::string, storage_engine_reference_handle*>* storage_engine_references_mapping)
{
    //
    // Create the object containers column family on the storage engine.
    //
    storage_engine_reference_handle* containers_internal_metadata_storage_engine_reference;
    status::status_code status = storage_engine_->create_container(
        container_index::containers_internal_metadata_name,
        &containers_internal_metadata_storage_engine_reference);

    if (status::failed(status))
    {
        spdlog::critical("Failed to create internal metadata column family '{}'.",
                         container_index::containers_internal_metadata_name);

        return status;
    }

    //
    // Append to the storage references mapping.
    //
    storage_engine_references_mapping->emplace(
        container_index::containers_internal_metadata_name,
        containers_internal_metadata_storage_engine_reference);

    return status::success;
}

void
container_management_service::orchestrate_serial_container_operation(
    schemas::container_request&& container_request,
    network::server_response_callback&& response_callback)
{
    container_operation_serializer_->enqueue_container_operation(
        std::move(container_request),
        std::move(response_callback));
}

status::status_code
container_management_service::validate_container_operation_request(
    const schemas::container_request& container_request)
{
    status::status_code status = common::request_validations::validate_container_name(
        container_request.get_name(),
        storage_configuration_);

    if (status::failed(status))
    {
        //
        // Given object container name is invalid.
        // Not logging the container name as to avoid potential
        // large-buffer attacks in case the name is too big.
        //
        spdlog::error("Object container operation will be failed as the "
            "object container name is invalid. "
            "Optype={}, "
            "ObjectContainerNameSizeInBytes={}, "
            "ObjectContainerNameMaxSizeInBytes={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name().size(),
            storage_configuration_.max_container_name_size_bytes_,
            status);

        return status;
    }

    //
    // Only log the request parameters after
    // they have been validated to be well-formed.
    //
    switch (container_request.get_optype())
    {
        case schemas::container_request_optype::create:
        {
            return validate_container_create_request(container_request);
            break;
        }
        case schemas::container_request_optype::remove:
        {
            return validate_container_remove_request(container_request);
            break;
        }
        default:
        {
            spdlog::error("Invalid optype received for object operation. "
                "Optype={}, "
                "ObjectContainerName={}.",
                static_cast<std::uint8_t>(container_request.get_optype()),
                container_request.get_name());

            return status::invalid_operation;
            break;
        }
    }

    return status::unreachable;
}

status::status_code
container_management_service::validate_container_create_request(
    const schemas::container_request& container_request)
{
    const status::status_code status =
        container_index_->get_container_existence_status(
            container_request.get_name());

    if (status != status::container_not_exists)
    {
        //
        // Fail fast in case the object container already exists.
        //
        spdlog::error("Object container creation will be failed as the "
            "object container is in a non-creatable state. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            status);

        return status;
    }

    if (container_index_->get_total_number_containers() >=
        storage_configuration_.max_number_containers_)
    {
        //
        // The total number of object containers present on the
        // system exceeds the limit. Fail the new creation operation.
        //
        spdlog::error("Object container creation will be failed as the "
            "current number of object containers exceeds the limit. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "TotalNumberOfObjectContainers={}, "
            "MaxNumberOfObjectContainers={}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            container_index_->get_total_number_containers(),
            storage_configuration_.max_number_containers_);

        return status::max_number_containers_reached;
    }

    return status::success;
}

status::status_code
container_management_service::validate_container_remove_request(
    const schemas::container_request& container_request)
{
    const status::status_code status =
        container_index_->get_container_existence_status(
            container_request.get_name());

    if (status != status::container_already_exists)
    {
        //
        // Fail fast in case the object container does not exist.
        //
        spdlog::error("Object container removal will be failed as the "
            "object container is in a non-deletable state. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            status);

        return status;
    }

    return status::success;
}

} // namespace storage.
} // namespace lazarus.