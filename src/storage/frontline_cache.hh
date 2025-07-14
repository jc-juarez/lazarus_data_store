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

class frontline_cache
{
public:

    //
    // Constructor.
    //
    frontline_cache(
        const std::uint16_t number_cache_shards,
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
};

} // namespace lazarus::storage.