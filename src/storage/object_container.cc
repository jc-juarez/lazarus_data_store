// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container.hh'
// Author: jcjuarez
// Description:
//      Object container metadata structure. Object
//      containers are logical buckets for holding
//      closely related data inside the data store. 
// ****************************************************

#include <format>
#include "storage_engine.hh"
#include "object_container.hh"

namespace lazarus
{
namespace storage
{

object_container::object_container(
    std::shared_ptr<storage_engine> storage_engine_handle,
    storage_engine_reference_handle* storage_engine_reference,
    const schemas::object_container_persistent_interface& object_container_persistent_metadata)
    : storage_engine_{std::move(storage_engine_handle)},
      storage_engine_reference_{storage_engine_reference},
      object_container_persistent_metadata_{object_container_persistent_metadata},
      is_deleted_{false}
{}

object_container::~object_container()
{
    storage_engine_->close_object_container_storage_engine_reference(
        storage_engine_reference_);
}

schemas::object_container_persistent_interface
object_container::create_object_container_persistent_metadata(
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
object_container::get_storage_engine_reference() const
{
    std::shared_lock<std::shared_mutex> {lock_};
    return storage_engine_reference_;
}

void
object_container::mark_as_deleted()
{
    std::unique_lock<std::shared_mutex> {lock_};
    is_deleted_ = true;
}

bool
object_container::is_deleted() const
{
    std::shared_lock<std::shared_mutex> {lock_};
    return is_deleted_;
}

std::string
object_container::to_string() const
{
    std::shared_lock<std::shared_mutex> {lock_};
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