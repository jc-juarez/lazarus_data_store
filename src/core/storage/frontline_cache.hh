// ****************************************************
// Lazarus Data Store
// Storage
// 'frontline_cache.hh'
// Author: jcjuarez
// Description:
//      LRU cache for storing recently accessed
//      objects from the server context within shards.
// ****************************************************

#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "cache_shard.hh"

namespace lazarus::storage
{

class container_index;

class frontline_cache
{
public:

    //
    // Constructor.
    //
    frontline_cache(
        const std::uint16_t number_cache_shards,
        const std::size_t max_cache_shard_size_bytes,
        const std::size_t max_object_size_bytes,
        std::shared_ptr<container_index> container_index);

    //
    // Inserts an object into the cache shard.
    // Note that the method takes the parameters are movable objects under the core
    // assumption that this must only be called post a write operation and that the references
    // being moved into this method will not be used post-call of this method.
    //
    status::status_code
    put(
        std::string&& object_id,
        byte_stream&& object_data,
        std::string&& container_name);

    //
    // Gets an object data if present in the cache.
    // Note that if the object is not present in the cache, it will not be inserted.
    // This is a design choice as this is called from the server threads and there is no
    // need to consume computation resources to insert it at the step of checking if an entry exists.
    // Eventually, if the object needs to be inserted, it will be inserted by the IO thread pools.
    //
    std::optional<byte_stream>
    get(
        const std::string& object_id,
        const std::string& container_name);

private:

    //
    // Gets the cache shard index by hashing the given object ID.
    //
    std::uint16_t
    get_associated_cache_shard_index(
        const std::string& object_id) const;

    //
    // Table for storing the internal cache shards.
    //
    std::vector<std::unique_ptr<cache_shard>> cache_shards_table_;

    //
    // Hasher for routing an object ID to its respective cache shard.
    //
    std::hash<std::string> hasher_;

    //
    // Number of shards in the cache.
    //
    const std::uint16_t number_cache_shards_;

    //
    // Container index component handle.
    //
    std::shared_ptr<container_index> container_index_;
};

} // namespace lazarus::storage.