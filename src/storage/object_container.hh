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

#pragma once

#include <string>
#include <shared_mutex>
#include "../common/aliases.hh"
#include "object_container_persistent_interface.pb.h"

namespace lazarus
{
namespace storage
{

class storage_engine;

//
// Object container metadata internal structure.
// An in-memory creation of an object container reference can
// only be created after a well-known committed disk write its creation.
//
class object_container
{
public:

    //
    // Constructor for the object container.
    //
    object_container(
        std::shared_ptr<storage_engine> storage_engine_handle,
        storage_engine_reference_handle* storage_engine_reference,
        const schemas::object_container_persistent_interface& object_container_persistent_metadata);

    //
    // Destructor for the object container.
    // Ensures that the storage engine reference is invalidated.
    //
    ~object_container();

    //
    // Initializes an object container persistent metadata instance
    // with default values and returns it to the caller.
    // Should be used for all new object container creations.
    //
    static
    schemas::object_container_persistent_interface
    create_object_container_persistent_metadata(
        const char* object_container_name);

    //
    // Gets the associated storage engine reference for the object container.
    //
    storage_engine_reference_handle*
    get_storage_engine_reference() const;

    //
    // Marks the object container as deleted.
    //
    void
    mark_as_deleted();

    //
    // Gets the deletion state of the object container.
    //
    bool
    is_deleted() const;

    //
    // Returns the object container contents in a string format.
    //
    std::string
    to_string() const;

private:

    //
    // Handle for the storage enine.
    //
    std::shared_ptr<storage_engine> storage_engine_;

    //
    // Object container persistent metadata.
    //
    schemas::object_container_persistent_interface object_container_persistent_metadata_;

    //
    // Pointer to the associated column family for the object container.
    // This only concerns the storage engine. In-memory only.
    //
    storage_engine_reference_handle* const storage_engine_reference_;

    //
    // Flag indicating whether this object container reference has been
    // marked for deletion. In-memory only.
    // This state does not need to be persisted as it is only set
    // once the storage engine has deleted the internal filesystem
    // metadata reference for the object container.
    //
    bool is_deleted_;

    //
    // Lock for synchronizing access to the object.
    //
    mutable std::shared_mutex lock_;
};

} // namespace storage.
} // namespace lazarus.