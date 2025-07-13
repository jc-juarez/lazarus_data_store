// ****************************************************
// Lazarus Data Store
// Storage
// 'container.hh'
// Author: jcjuarez
// Description:
//      Object container metadata structure. Object
//      containers are logical buckets for holding
//      closely related data inside the data store. 
// ****************************************************

#include <format>
#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "container.hh"

namespace lazarus
{
namespace storage
{

container::container(
    std::shared_ptr<storage_engine> storage_engine_handle,
    storage_engine_reference_handle* storage_engine_reference,
    const schemas::object_container_persistent_interface& object_container_persistent_metadata)
    : storage_engine_{std::move(storage_engine_handle)},
      storage_engine_reference_{storage_engine_reference},
      object_container_persistent_metadata_{object_container_persistent_metadata},
      is_deleted_{false}
{}

container::~container()
{
    spdlog::info("Removing last object container reference from memory. "
        "ObjectContainerMetadata={}.",
        to_string());

    storage_engine_->close_object_container_storage_engine_reference(
        storage_engine_reference_);
}

schemas::object_container_persistent_interface
container::create_object_container_persistent_metadata(
    const char* object_container_name)
{
    schemas::object_container_persistent_interface object_container_persistent_metadata;

    //
    // Default values upon creation.
    //
    object_container_persistent_metadata.set_name(object_container_name);

    return object_container_persistent_metadata;
}

storage_engine_reference_handle*
container::get_storage_engine_reference() const
{
    std::shared_lock<std::shared_mutex> lock {lock_};
    return storage_engine_reference_;
}

std::string
container::get_name() const
{
    std::shared_lock<std::shared_mutex> lock {lock_};
    return object_container_persistent_metadata_.name();
}

void
container::mark_as_deleted()
{
    std::unique_lock<std::shared_mutex> lock {lock_};
    is_deleted_ = true;
}

bool
container::is_deleted() const
{
    std::shared_lock<std::shared_mutex> lock {lock_};
    return is_deleted_;
}

std::string
container::to_string() const
{
    std::shared_lock<std::shared_mutex> lock {lock_};
    return std::format(
        "{{Name={}, "
        "StorageEngineReference={}, "
        "IsDeleted={}}}",
        object_container_persistent_metadata_.name(),
        static_cast<void*>(storage_engine_reference_),
        is_deleted_);
}

} // namespace storage.
} // namespace lazarus.