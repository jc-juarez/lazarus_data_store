// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container_index.hh'
// Author: jcjuarez
// Description:
//      Indexes and routes object containers for all
//      data and metadata operations. 
// ****************************************************

#include <cassert>
#include <spdlog/spdlog.h>
#include "object_container_index.hh"

namespace lazarus
{
namespace storage
{

object_container_index::object_container_index()
{}

void
object_container_index::fetch_object_containers_from_disk()
{}

void
object_container_index::insert_object_container(
    const lazarus::schemas::object_container_persistance_interface& object_container_persistance)
{
    //
    // At this point, it is guaranteed that the object container persistance
    // reference has a valid hashable identifier to be used as index key.
    // Also, it is guaranteed that no other thread will try to insert the same key.
    //
    if (object_container_index_map_.insert({
        object_container_persistance.id(),
        object_container{object_container_persistance}}))
    {
        //
        // Key did not exist and metadata register was succesful.
        // Exit execution.
        //
        return;
    }

    //
    // Reaching this point is a critical error as no other thread
    // should have reached this point before for inserting the same key.
    // Log the issue and assert.
    //
    spdlog::critical("Object container collision has been detected");
    assert(false);
}

} // namespace storage.
} // namespace lazarus.