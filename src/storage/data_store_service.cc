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
#include "object_container_persistance_interface.pb.h"

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
    object_container_index_ = std::make_shared<object_container_index>();

    //
    // Object container operation serializer component allocation.
    //
    object_container_operation_serializer_ = std::make_shared<object_container_operation_serializer>(
        storage_engine_,
        object_container_index_);
}

void
data_store_service::populate_object_container_index(
    const std::unordered_map<std::string, rocksdb::ColumnFamilyHandle*>& column_family_references_mapping)
{
    //
    // Keep track of the internal metadata object containers.
    // The only case when it is valid for them to be absent is when the column
    // family references mapping only has 1 entry (kDefaultColumnFamilyName), which
    // implies that the data store is starting up for the first time. In any other
    // case, it implies that the system cannot start because of missing critical
    // internal metadata.
    //
    if (column_family_references_mapping.size() == 1u)
    {
        // This means this is the first ever startup for the data store,
        // or that it simply does not need a previous metadata state for working.
        // Create all required internal metadata root object containers and exit.
        create_internal_metadata_column_families();
        return;
    }

    //
    // This forcefully means that this is not the first time data store
    // startup. Ensure that all internal metadata object containers are present; if not,
    // this is a critical error as the data store cannot function without them.
    //
    const std::optional<std::pair<std::string, rocksdb::ColumnFamilyHandle*>> object_containers_internal_metadata =
        find_object_containers_internal_metadata_association_pair(column_family_references_mapping);

    if (!object_containers_internal_metadata.has_value())
    {
        // This is a critical error as one or some of the core internal metadata
        // column families were not found. Fail the system startup.
        spdlog::critical("Failed to find find the storage engine reference for the '{}' internal metadata.",
            object_container_index::object_containers_internal_metadata_column_family_name);

        throw std::runtime_error("Failed to find find the storage engine reference for internal metadata.");
    }

    lazarus::schemas::object_container_persistance_interface object_container_persistent_data;
    object_container_persistent_data.set_name(object_container_index::object_containers_internal_metadata_column_family_name);
    object_container_index_->set_object_containers_internal_metadata_handle(
        object_containers_internal_metadata.value().second,
        object_container_persistent_data);

    //
    // Finally, get all known object containers to the system
    // from the storage engine and populate the rest of the object container index.
    //
    const std::unordered_map<std::string, byte_stream> objects = 
        storage_engine_->get_all_objects_from_object_container(
            object_container_index_->get_object_containers_internal_metadata_data_store_reference());

    for (const auto& object : objects)
    {
        lazarus::schemas::object_container_persistance_interface object_container_persistent_data;
        const bool is_parsing_successful = object_container_persistent_data.ParseFromString(object.second);

        if (!is_parsing_successful)
        {
            spdlog::critical("Failed to parse an object container metadata on startup. "
                "ObjectContainerName={}.",
                object.first);

            throw std::runtime_error("Failed to parse an object container metadata on startup.");
        }

        if (column_family_references_mapping.find(object_container_persistent_data.name()) == column_family_references_mapping.end())
        {
            //
            // The initial mapping obtained from the storage engine does not
            // contain a storage engine reference for an object container known
            // to the object containers internal metadata column family. This is a
            // critical error and the data store cannot start without this information.
            //
            spdlog::critical("Failed to find the storage engine reference for an object container on startup. "
                "ObjectContainerName={}.",
                object.first);

            throw std::runtime_error("Failed to find the storage engine reference for an object container on startup.");
        }

        object_container_index_->insert_object_container(
            column_family_references_mapping.at(object_container_persistent_data.name()),
            object_container_persistent_data);
    }
}

void
data_store_service::create_internal_metadata_column_families()
{
    //
    // Create the object containers column family
    // and assign the internal metadata handle.
    //
    rocksdb::ColumnFamilyHandle* storage_engine_reference =
        storage_engine_->create_object_container(
            object_container_index::object_containers_internal_metadata_column_family_name);

    lazarus::schemas::object_container_persistance_interface object_container_persistent_data;
    object_container_persistent_data.set_name(object_container_index::object_containers_internal_metadata_column_family_name);
    object_container_index_->set_object_containers_internal_metadata_handle(
        storage_engine_reference,
        object_container_persistent_data);
}

void
data_store_service::enqueue_async_object_insertion(
    const char* object_id,
    const byte* object_data_stream,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
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
        callback = std::move(callback)]() mutable
        {
            this->object_insertion_dispatch_proxy(
                std::move(object_id_to_dispatch),
                std::move(object_data_stream_to_dispatch),
                std::move(callback));
        });
}

void
data_store_service::get_object(
    const char* object_id,
    byte_stream& object_data_stream)
{
    storage_engine_->get_object(
        object_id,
        object_data_stream);
}

void
data_store_service::orchestrate_serial_object_container_operation(
    lazarus::schemas::object_container_request_interface&& object_container_request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    object_container_operation_serializer_->enqueue_object_container_operation(
        std::move(object_container_request),
        std::move(callback));
}

std::optional<std::pair<std::string, rocksdb::ColumnFamilyHandle*>>
data_store_service::find_object_containers_internal_metadata_association_pair(
    const std::unordered_map<std::string, rocksdb::ColumnFamilyHandle*>& column_family_references_mapping)
{
    for (const auto& column_family_mapping_entry : column_family_references_mapping)
    {
        const std::string object_container_name = column_family_mapping_entry.first;
        rocksdb::ColumnFamilyHandle* object_container_data_store_reference = column_family_mapping_entry.second;

        if (object_container_name == object_container_index::object_containers_internal_metadata_column_family_name)
        {
            // This entry corresponds to the object containers internal metadata column family.
            return std::optional<std::pair<std::string, rocksdb::ColumnFamilyHandle*>>(
                std::make_pair(
                    object_container_name,
                    object_container_data_store_reference));
        }
    }

    return std::nullopt;
}

void
data_store_service::object_insertion_dispatch_proxy(
    const std::string&& object_id,
    const byte_stream&& object_data_stream,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    //
    // At this point, it is guaranteed that the memory contents
    // of the object are safely copied ino the provided containers.
    //
    storage_engine_->insert_object(
        object_id.c_str(),
        object_data_stream.c_str());

    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setBody(
        "Async response. Object has been inserted.");
    
    callback(resp);
}

} // namespace storage.
} // namespace lazarus.