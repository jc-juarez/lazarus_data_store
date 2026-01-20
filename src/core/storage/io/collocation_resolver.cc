// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
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
    const std::string& object_key)
{
    return hasher_(object_key) % number_data_collocations_;
}

} // namespace storage.
} // namespace lazarus.