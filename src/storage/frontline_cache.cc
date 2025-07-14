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

namespace lazarus::storage
{

frontline_cache::frontline_cache(
    const std::uint16_t number_cache_shards,
    const std::size_t max_cache_shard_size_bytes,
    const std::size_t max_object_size_bytes)
    : number_cache_shards_{number_cache_shards}
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
    byte_stream&& object_data)
{
    const std::uint16_t cache_shard_index =
        get_associated_cache_shard_index(object_id);

    return cache_shards_table_.at(cache_shard_index)->put(
        std::move(object_id),
        std::move(object_data));
}

std::optional<byte_stream>
frontline_cache::get(
    const std::string& object_id)
{
    const std::uint16_t cache_shard_index =
        get_associated_cache_shard_index(object_id);

    return cache_shards_table_.at(cache_shard_index)->get(
        object_id);
}

std::uint16_t
frontline_cache::get_associated_cache_shard_index(
    const std::string& object_id) const
{
    return hasher_(object_id) % number_cache_shards_;
}

} // namespace lazarus::storage.