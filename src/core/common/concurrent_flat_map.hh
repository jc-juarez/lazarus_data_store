// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Common
// 'concurrent_flat_map.hh'
// Author: jcjuarez
// Description:
//      Thread-safe hash map with a swiss table
//      implementation.
// ****************************************************

#include <vector>
#include <optional>
#include <shared_mutex>
#include <absl/container/flat_hash_map.h>

namespace lazarus
{
namespace common
{

template<typename K, typename V>
class concurrent_flat_map
{
public:

    //
    // Constructor.
    //
    concurrent_flat_map(
        const std::uint16_t num_map_shards = k_default_num_map_shards)
        : map_shards_{num_map_shards},
          num_map_shards_{num_map_shards}
    {}

    //
    // Inserts an element into the map.
    //
    void
    insert(
        const K& key,
        const V& value)
    {
        auto& map_shard = get_map_shard(key);
        std::unique_lock<std::shared_mutex> lock(map_shard.lock_);
        map_shard.flat_hash_map_[key] = value;
    }

    //
    // Returns the value for a specified key if present.
    //
    std::optional<V>
    get(
        const K& key)
    {
        auto& map_shard = get_map_shard(key);
        std::shared_lock<std::shared_mutex> lock(map_shard.lock_);

        auto it = map_shard.flat_hash_map_.find(key);
        if (it == map_shard.flat_hash_map_.end())
        {
            return std::nullopt;
        }

        return it->second;
    }

    //
    // Checks whether a particular entry exists.
    //
    bool
    exists(
        const K& key)
    {
        auto& map_shard = get_map_shard(key);
        std::shared_lock<std::shared_mutex> lock(map_shard.lock_);
        return map_shard.flat_hash_map_.find(key) != map_shard.flat_hash_map_.end();
    }

    //
    // Deletes an entry from the map.
    //
    bool
    erase(
        const K& key)
    {
        auto& map_shard = get_map_shard(key);
        std::unique_lock<std::shared_mutex> lock(map_shard.lock_);
        return map_shard.flat_hash_map_.erase(key) > 0;
    }

private:

    //
    // Map shard.
    // Owns the locking mutex and the swiss table flat hash map.
    // External owner class will manage access to the internal lock.
    //
    struct map_shard
    {
        std::shared_mutex lock_;
        absl::flat_hash_map<K, V> flat_hash_map_;
    };

    //
    // Gets a reference to the respective shard.
    //
    map_shard&
    get_map_shard(
        const K& key)
    {
        return map_shards_.at(hasher_(key) % num_map_shards_);
    }

    //
    // Default number of map shards.
    //
    static constexpr std::uint16_t k_default_num_map_shards = 64u;

    //
    // Hasher for routing a key to its respective map shard.
    //
    std::hash<K> hasher_;

    //
    // List of flat map shards.
    //
    std::vector<map_shard> map_shards_;

    //
    // Number of shards for the map.
    //
    const std::uint16_t num_map_shards_;
};

} // namespace common.
} // namespace lazarus.