// ****************************************************
// Lazarus Data Store
// Storage
// 'cache_shard.hh'
// Author: jcjuarez
// Description:
//      Fundamental cache shard for the frontline
//      cache.
// ****************************************************

#pragma once

#include <list>
#include <mutex>
#include <string>
#include <optional>
#include <unordered_map>
#include "../status/status.hh"
#include "../common/aliases.hh"

namespace lazarus::storage
{

class cache_shard
{
public:

    //
    // Constructor.
    //
    cache_shard(
        const std::size_t max_cache_shard_size_bytes,
        const std::size_t max_object_size_bytes);

    //
    // Inserts an object into the cache shard.
    // Note that the method takes the parameters are movable objects under the core
    // assumption that this must only be called post a write operation and that the references
    // being moved into this method will not be used post-call of this method.
    //
    status::status_code
    put(
        std::string&& object_id,
        byte_stream&& object_data);

    //
    // Gets an object data if present in the cache.
    //
    std::optional<byte_stream>
    get(
        const std::string& object_id);

private:

    //
    // Max cache shard size in bytes.
    //
    const std::size_t max_cache_shard_size_bytes_;

    //
    // Max size for objects to be inserted into the cache shard.
    //
    const std::size_t max_object_size_bytes_;

    //
    // Current size of the cache shard in bytes.
    //
    std::size_t current_cache_shard_size_bytes_;

    //
    // Doubly linked list chain for storing the node objects in a flat structure for direct dereferencing.
    //
    std::list<std::pair<std::string, byte_stream>> lru_doubly_linked_list_;

    //
    // Quick-access cache map for directly dereferencing the node object.
    //
    std::unordered_map<std::string, std::list<std::pair<std::string, byte_stream>>::iterator> lru_cache_map_;

    //
    // Lock for synchronizing access to the cache shard.
    //
    mutable std::mutex lock_;
};

} // namespace lazarus::storage.