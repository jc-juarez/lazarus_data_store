// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container_index.hh'
// Author: jcjuarez
// Description:
//      Indexes and routes object containers for all
//      data and metadata operations. 
// ****************************************************

#pragma once

#include <tbb/tbb.h>
#include "object_container.hh"
#include "object_container_persistance_interface.pb.h"

namespace lazarus
{
namespace storage
{

//
// Core object container indexing system.
//
class object_container_index
{
public:

    //
    // Constructor for the object container index.
    //
    object_container_index();

    //
    // Fetches the existing object container references on the filesystem
    // into memory. This should only be executed during system startup.
    // In case of a system crash, the underlying storage engine ensures
    // that all operations are written into the WAL entries, thus lazarus
    // only acknowledges object container references that were successfully
    // recorded into the storage engine WAL.
    //
    void
    fetch_object_containers_from_disk();

    //
    // Inserts a new object container entry into the index map.
    // This can either be invoked as a callback from a request-initiated
    // object container insertion or as the initial disk fetching process.
    // This is only executed after a well-known commited disk write.
    //
    void
    insert_object_container(
        const lazarus::schemas::object_container_persistance_interface& object_container_persistance);

private:

    //
    // Main index for object containers in the system.
    // Maps an object container identifier to the
    // respective object container memory reference.
    //
    tbb::concurrent_hash_map<std::string, object_container> object_container_index_map_;
};

} // namespace storage.
} // namespace lazarus.