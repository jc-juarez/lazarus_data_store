// ****************************************************
// Lazarus Data Store
// Storage
// 'frontline_cache.hh'
// Author: jcjuarez
// Description:
//      LRU cache for storing recently accessed
//      objects from the server context within shards.
// ****************************************************

#include "frontline_cache.hh"
#include "container_index.hh"

namespace lazarus::storage
{

frontline_cache::frontline_cache(
    const std::uint16_t number_cache_shards,
    const std::size_t max_cache_shard_size_bytes,
    const std::size_t max_object_size_bytes,
    std::shared_ptr<container_index> container_index_handle)
    : number_cache_shards_{number_cache_shards},
      container_index_handle_{std::move(container_index_handle)}
{
    for (std::uint16_t index = 0; index < number_cache_shards_; ++index)
    {
        cache_shards_table_.emplace_back(
            std::make_unique<cache_shard>(max_cache_shard_size_bytes, max_object_size_bytes));
    }
}

status::status_code
frontline_cache::put(
    std::string&& object_id,
    byte_stream&& object_data,
    std::string&& container_name)
{
    const std::uint16_t cache_shard_index =
        get_associated_cache_shard_index(object_id);

    return cache_shards_table_.at(cache_shard_index)->put(
        std::move(object_id),
        std::move(object_data),
        std::move(container_name));
}

std::optional<byte_stream>
frontline_cache::get(
    const std::string& object_id,
    const std::string& container_name)
{
    const std::uint16_t cache_shard_index =
        get_associated_cache_shard_index(object_id);

    return cache_shards_table_.at(cache_shard_index)->get(
        object_id,
        container_name);
}

std::uint16_t
frontline_cache::get_associated_cache_shard_index(
    const std::string& object_id) const
{
    //
    // For simplicity, the cache shard will be decided by only hashing the
    // object ID. This implies that identical object IDs from different
    // containers will land at the same shard.
    //
    return hasher_(object_id) % number_cache_shards_;
}

} // namespace lazarus::storage.