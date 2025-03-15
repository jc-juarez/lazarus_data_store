// ****************************************************
// Lazarus Data Store
// Storage
// 'data_store_service.cc'
// Author: jcjuarez
// Description:
//      Accessor core storage operations. 
// ****************************************************

#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "data_store_service.hh"
#include "object_container_index.hh"
#include "object_container_operation_serializer.hh"
#include "object_container_persistent_interface.pb.h"

namespace lazarus
{
namespace storage
{

data_store_service::data_store_service(
    std::shared_ptr<storage_engine> storage_engine_handle)
    : storage_engine_(std::move(storage_engine_handle)),
      object_insertion_thread_pool_{16u}
{
    //
    // Object container index component allocation.
    //
    object_container_index_ = std::make_shared<object_container_index>(
        storage_engine_);

    //
    // Object container operation serializer component allocation.
    //
    object_container_operation_serializer_ = std::make_shared<object_container_operation_serializer>(
        storage_engine_,
        object_container_index_);
}

status::status_code
data_store_service::populate_object_container_index(
    std::unordered_map<std::string, rocksdb::ColumnFamilyHandle*>* storage_engine_references_mapping)
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

    rocksdb::ColumnFamilyHandle* object_containers_internal_metadata_storage_engine_reference =
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
        const std::string& object_container_name = storage_engine_references_pair.first;
        rocksdb::ColumnFamilyHandle* object_container_storage_engine_reference = storage_engine_references_pair.second;

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

                return status::object_parsing_failed;
            }

            spdlog::info("Found object container on startup. Indexing into the object containers metadata table. "
                "ObjectContainerName={}.",
                object_container_persistent_metadata.name());

            object_container_index_->insert_object_container(
                object_container_storage_engine_reference,
                object_container_persistent_metadata);
        }
        else
        {
            //
            // This implies this is not a known object container to the persistent metadata.
            // In this case, the only object containers to index should be the internal metadata.
            //
            if (object_container_index::is_internal_metadata(object_container_name.c_str()))
            {
                //
                // Index the internal metadata object containers.
                //
                schemas::object_container_persistent_interface object_container_persistent_metadata;
                object_container_persistent_metadata.set_name(object_container_name);
                object_container_index_->insert_object_container(
                    object_container_storage_engine_reference,
                    object_container_persistent_metadata);
            }
            else
            {
                //
                // If this is not part of the internal metadata, it means that this is an
                // orphaned object container; the garbage collector will clean it up later.
                //
                spdlog::warn("Found orphaned object container on startup. "
                    "ObjectContainerName={}.",
                    object_container_name);
            }
        }
    }

    return status::success;
}

status::status_code
data_store_service::create_internal_metadata_column_families(
    std::unordered_map<std::string, rocksdb::ColumnFamilyHandle*>* storage_engine_references_mapping)
{
    //
    // Create the object containers column family on the storage engine.
    //
    rocksdb::ColumnFamilyHandle* object_containers_internal_metadata_storage_engine_reference;
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
data_store_service::enqueue_async_object_insertion(
    const char* object_id,
    const byte* object_data_stream,
    network::server_response_callback&& response_callback)
{
    //
    // Ensure the associated contents of the object are copied before proceeding. 
    //
    std::string object_id_to_dispatch{object_id};
    byte_stream object_data_stream_to_dispatch{object_data_stream};

    //
    // Enqueue the object insertion action.
    //
    object_insertion_thread_pool_.execute(
        [this,
        object_id_to_dispatch = std::move(object_id_to_dispatch),
        object_data_stream_to_dispatch = std::move(object_data_stream_to_dispatch),
        response_callback = std::move(response_callback)]() mutable
        {
            this->object_insertion_dispatch_proxy(
                std::move(object_id_to_dispatch),
                std::move(object_data_stream_to_dispatch),
                std::move(response_callback));
        });
}

void
data_store_service::get_object(
    const char* object_id,
    byte_stream& object_data)
{
    //storage_engine_->get_object(
    //    object_id,
    //    object_data_stream);
}

void
data_store_service::orchestrate_serial_object_container_operation(
    schemas::object_container_request_interface&& object_container_request,
    network::server_response_callback&& response_callback)
{
    object_container_operation_serializer_->enqueue_object_container_operation(
        std::move(object_container_request),
        std::move(response_callback));
}

bool
data_store_service::object_container_exists(
    const char* object_container_name)
{
    return object_container_index_->object_container_exists(
        object_container_name);
}

status::status_code
data_store_service::validate_object_container_operation_request(
    const schemas::object_container_request_interface& object_container_request)
{
    switch (object_container_request.get_optype())
    {
        case schemas::object_container_request_optype::create:
        {
            if (object_container_index_->object_container_exists(
                object_container_request.get_name()))
            {
                //
                // Fail fast in case the object container already exists.
                //
                spdlog::error("Object container creation will be failed as the "
                    "object container already exists. "
                    "Optype={}, "
                    "ObjectContainerName={}.",
                    static_cast<std::uint8_t>(object_container_request.get_optype()),
                    object_container_request.get_name());

                return status::object_container_already_exists;
            }

            break;
        }
        case schemas::object_container_request_optype::remove:
        {
            if (!object_container_index_->object_container_exists(
                object_container_request.get_name()))
            {
                //
                // Fail fast in case the object container does not exist.
                //
                spdlog::error("Object container removal will be failed as the "
                    "object container does not exist. "
                    "Optype={}, "
                    "ObjectContainerName={}.",
                    static_cast<std::uint8_t>(object_container_request.get_optype()),
                    object_container_request.get_name());

                return status::object_container_not_exists;
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

void
data_store_service::object_insertion_dispatch_proxy(
    const std::string&& object_id,
    const byte_stream&& object_data_stream,
    network::server_response_callback&& response_callback)
{
    //
    // At this point, it is guaranteed that the memory contents
    // of the object are safely copied ino the provided containers.
    //
    //storage_engine_->insert_object(
    //    object_id.c_str(),
    //    object_data_stream.c_str());

    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setBody(
        "Async response. Object has been inserted.");
    
    response_callback(resp);
}

} // namespace storage.
} // namespace lazarus.