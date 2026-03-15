// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'collocation_resolver.cc'
// Author: jcjuarez
// Description:
//      Routes object keys to the corresponding
//      collocation index to be used by providers.
// ****************************************************

#include <xxhash.h>
#include "collocation_resolver.hh"

namespace lazarus
{
namespace storage
{

collocation_resolver::collocation_resolver(
    const std::uint16_t number_data_collocations)
    : number_data_collocations_{number_data_collocations}
{}

std::uint16_t
collocation_resolver::get_collocation_index_for_key(
    const std::string& object_key) const
{
    return static_cast<std::uint16_t>(
        hash_key(object_key) % number_data_collocations_);
}

std::uint64_t
collocation_resolver::hash_key(
    const std::string& key)
{
    //
    // Do not change.
    // Using stable and deterministic hashing.
    // This will yield a universal persistent result.
    //
    return XXH3_64bits(key.data(), key.size());
}

} // namespace storage.
} // namespace lazarus.