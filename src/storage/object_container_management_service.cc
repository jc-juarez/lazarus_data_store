// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container_management_service.cc'
// Author: jcjuarez
// Description:
//      Accessor core storage operations. 
// ****************************************************

#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "garbage_collector.hh"
#include "object_container_management_service.hh"
#include "object_container_index.hh"
#include "../common/uuid_utilities.hh"
#include "object_container_operation_serializer.hh"
#include "object_container_persistent_interface.pb.h"

namespace lazarus
{
namespace storage
{

object_container_management_service::object_container_management_service(
    std::shared_ptr<storage_engine> storage_engine_handle,
    std::shared_ptr<object_container_index> object_container_index_handle,
    std::unique_ptr<object_container_operation_serializer> object_container_operation_serializer_handle)
    : storage_engine_(std::move(storage_engine_handle)),
      object_container_index_{std::move(object_container_index_handle)},
      object_container_operation_serializer_{std::move(object_container_operation_serializer_handle)}
{}

status::status_code
object_container_management_service::populate_object_container_index(
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
    if (storage_engine_references_mapping->find(object_container_index::object_containers_internal_metadata_name) ==
        storage_engine_references_mapping->end())
    {
        //
        // This is a critical error as one or some of the core internal
        // metadata column families were not found. Fail the system startup.
        //
        spdlog::critical("Failed to find find the storage engine reference for the '{}' internal metadata.",
            object_container_index::object_containers_internal_metadata_name);

        return status::object_containers_internal_metadata_lookup_failed;
    }

    storage_engine_reference_handle* object_containers_internal_metadata_storage_engine_reference =
        storage_engine_references_mapping->at(object_container_index::object_containers_internal_metadata_name);

    //
    // Finally, get all known object containers to the system
    // from the storage engine and populate the rest of the object container index.
    //
    std::unordered_map<std::string, byte_stream> objects;
    status::status_code status = storage_engine_->get_all_objects_from_object_container(
        object_containers_internal_metadata_storage_engine_reference,
        &objects);

    if (status::failed(status))
    {
        spdlog::critical("Failed to get all object containers from the object containers internal metadata. "
            "Status={}.",
            status);

        return status;
    }

    for (const auto& storage_engine_references_pair : *storage_engine_references_mapping)
    {
        std::string object_container_name = storage_engine_references_pair.first;
        storage_engine_reference_handle* object_container_storage_engine_reference = storage_engine_references_pair.second;

        if (objects.find(object_container_name) != objects.end())
        {
            //
            // This implies this is a known object container to the persistent metadata.
            //
            byte_stream& object_data = objects.at(object_container_name);
            schemas::object_container_persistent_interface object_container_persistent_metadata;
            const bool is_parsing_successful = object_container_persistent_metadata.ParseFromString(object_data);

            if (!is_parsing_successful)
            {
                spdlog::critical("Failed to parse an object container metadata on startup. "
                    "ObjectContainerName={}.",
                    object_container_name);

                return status::parsing_failed;
            }

            object_container_index_->insert_object_container(
                object_container_storage_engine_reference,
                object_container_persistent_metadata);

            const std::shared_ptr<object_container> object_container =
                object_container_index_->get_object_container(object_container_name.c_str());

            spdlog::info("Found object container on startup. Indexing into the object containers metadata table. "
                "ObjectContainerMetadata={}.",
                object_container->to_string());
        }
        else
        {
            //
            // This implies this is not a known object container to the persistent metadata.
            // These could be internal metadata or orphaned object containers.
            //
            const schemas::object_container_persistent_interface object_container_persistent_metadata =
                object_container::create_object_container_persistent_metadata(object_container_name.c_str());
            object_container_index_->insert_object_container(
                object_container_storage_engine_reference,
                object_container_persistent_metadata);

            const bool is_orphaned_object_container =
                !object_container_index::is_internal_metadata(object_container_name.c_str());

            if (is_orphaned_object_container)
            {
                //
                // If this is not part of the internal metadata, it means that this is an
                // orphaned object container; mark it as deleted for the garbage collector to clean it up later.
                //
                spdlog::warn("Found orphaned object container on startup to be cleaned up by the garbage collector. "
                    "ObjectContainerName={}.",
                    object_container_name.c_str());
                
                std::shared_ptr<object_container> object_container = 
                    object_container_index_->get_object_container(object_container_name.c_str());

                if (object_container == nullptr)
                {
                    spdlog::critical("Failed to mark object container as deleted on startup. "
                        "ObjectContainerName={}.",
                        object_container_name.c_str());

                    return status;
                }

                object_container->mark_as_deleted();
            }
        }
    }

    return status::success;
}

status::status_code
object_container_management_service::create_internal_metadata_column_families(
    std::unordered_map<std::string, storage_engine_reference_handle*>* storage_engine_references_mapping)
{
    //
    // Create the object containers column family on the storage engine.
    //
    storage_engine_reference_handle* object_containers_internal_metadata_storage_engine_reference;
    status::status_code status = storage_engine_->create_object_container(
        object_container_index::object_containers_internal_metadata_name,
        &object_containers_internal_metadata_storage_engine_reference);

    if (status::failed(status))
    {
        spdlog::critical("Failed to create internal metadata column family '{}'.",
            object_container_index::object_containers_internal_metadata_name);

        return status;
    }

    //
    // Append to the storage references mapping.
    //
    storage_engine_references_mapping->emplace(
        object_container_index::object_containers_internal_metadata_name,
        object_containers_internal_metadata_storage_engine_reference);

    return status::success;
}

void
object_container_management_service::orchestrate_serial_object_container_operation(
    schemas::object_container_request_interface&& object_container_request,
    network::server_response_callback&& response_callback)
{
    object_container_operation_serializer_->enqueue_object_container_operation(
        std::move(object_container_request),
        std::move(response_callback));
}

status::status_code
object_container_management_service::validate_object_container_operation_request(
    const schemas::object_container_request_interface& object_container_request)
{
    switch (object_container_request.get_optype())
    {
        case schemas::object_container_request_optype::create:
        {
            const status::status_code status = 
                object_container_index_->get_object_container_existence_status(
                    object_container_request.get_name());

            if (status != status::object_container_not_exists)
            {
                //
                // Fail fast in case the object container already exists.
                //
                spdlog::error("Object container creation will be failed as the "
                    "object container is in a non-creatable state. "
                    "Optype={}, "
                    "ObjectContainerName={}, "
                    "Status={:#x}.",
                    static_cast<std::uint8_t>(object_container_request.get_optype()),
                    object_container_request.get_name(),
                    status);

                return status;
            }

            break;
        }
        case schemas::object_container_request_optype::remove:
        {
            const status::status_code status = 
                object_container_index_->get_object_container_existence_status(
                    object_container_request.get_name());

            if (status != status::object_container_already_exists)
            {
                //
                // Fail fast in case the object container does not exist.
                //
                spdlog::error("Object container removal will be failed as the "
                    "object container is in a non-deletable state. "
                    "Optype={}, "
                    "ObjectContainerName={}, "
                    "Status={:#x}.",
                    static_cast<std::uint8_t>(object_container_request.get_optype()),
                    object_container_request.get_name(),
                    status);

                return status;
            }

            break;
        }
        default:
        {
            return status::invalid_operation;
            break;
        }
    }

    return status::success;
}

} // namespace storage.
} // namespace lazarus.