// ****************************************************
// Lazarus Data Store
// Storage
// 'cache_shard.cc'
// Author: jcjuarez
// Description:
//      Fundamental LRU cache shard unit for the
//      frontline cache.
// ****************************************************

#include "cache_shard.hh"

namespace lazarus::storage
{

using container_object_id_pair = std::pair<std::string, std::string>;

cache_shard::cache_shard(
    const std::size_t max_cache_shard_size_bytes,
    const std::size_t max_object_size_bytes)
    : max_cache_shard_size_bytes_{max_cache_shard_size_bytes},
      max_object_size_bytes_{max_object_size_bytes},
      current_cache_shard_size_bytes_{0u}
{}

status::status_code
cache_shard::put(
    std::string&& object_id,
    byte_stream&& object_data,
    std::string&& container_name)
{
    //
    // Before taking the lock, execute the sanity check
    // to ensure the value does not exceed the limit size.
    //
    const std::size_t object_data_size_bytes = object_data.size();
    if (object_data_size_bytes > max_object_size_bytes_)
    {
        return status::object_data_size_exceeds_cache_limit;
    }

    //
    // Construct the key object before taking the lock.
    // The parameters will be moved under the assumption of no further usage.
    //
    container_object_id_pair object_id_pair = std::make_pair(
        std::move(container_name),
        std::move(object_id));

    std::lock_guard<std::mutex> lock {lock_};

    auto map_object_iterator = lru_cache_map_.find(object_id_pair);
    if (map_object_iterator != lru_cache_map_.end())
    {
        if (lru_doubly_linked_list_.begin() == lru_cache_map_.at(object_id_pair))
        {
            //
            // This means the object is already at the front of the list.
            // Even though no position change is needed, the actual object data
            // could be different from the one in the cache, so update the current value
            // with the one provided.
            //
            lru_doubly_linked_list_.begin()->second = std::move(object_data);
            return status::success;
        }

        //
        // If the object exists and is not at the front, remove it from the cache instead of splicing it.
        // This is more efficient given the cache does not know in advance how many
        // bytes need to be removed in object terms in order to insert this new object.
        //
        current_cache_shard_size_bytes_ -= map_object_iterator->second->second.size();
        lru_doubly_linked_list_.erase(map_object_iterator->second);
        lru_cache_map_.erase(map_object_iterator);
    }

    while ((current_cache_shard_size_bytes_ + object_data_size_bytes) > max_cache_shard_size_bytes_ &&
           !lru_doubly_linked_list_.empty())
    {
        //
        // Evict as many old elements as needed to insert the new object.
        // This can become detrimental if the new object has a large size and multiple removals are
        // needed, thus it is ideal to keep the maximum size for objects in the size as small as possible,
        // or at least in a big differential ratio between the max object size over the max cache shard size.
        //
        auto& oldest_object_in_list = lru_doubly_linked_list_.back();
        current_cache_shard_size_bytes_ -= oldest_object_in_list.second.size();
        lru_cache_map_.erase(oldest_object_in_list.first);
        lru_doubly_linked_list_.pop_back();
    }

    //
    // The data can be moved in the first call.
    //
    lru_doubly_linked_list_.emplace_front(object_id_pair, std::move(object_data));
    lru_cache_map_.emplace(object_id_pair, lru_doubly_linked_list_.begin());
    current_cache_shard_size_bytes_ += object_data_size_bytes;

    return status::success;
}

std::optional<byte_stream>
cache_shard::get(
    const std::string& object_id,
    const std::string& container_name)
{
    //
    // Construct the key object before taking the lock.
    //
    container_object_id_pair object_id_pair = std::make_pair(
        container_name,
        object_id);

    std::lock_guard<std::mutex> lock {lock_};

    auto map_object_iterator = lru_cache_map_.find(object_id_pair);
    if (map_object_iterator == lru_cache_map_.end())
    {
        return std::nullopt;
    }

    //
    // For get operations, splicing the cache shard is more efficient
    // as no elements need to be evicted given the size remains the same.
    //
    lru_doubly_linked_list_.splice(
        lru_doubly_linked_list_.begin(),
        lru_doubly_linked_list_,
        map_object_iterator->second);

    return std::make_optional<byte_stream>(map_object_iterator->second->second);
}

} // namespace lazarus::storage.