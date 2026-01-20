// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'collocation_resolver.hh'
// Author: jcjuarez
// Description:
//      Routes object keys to the corresponding
//      collocation index to be used by providers.
// ****************************************************

#pragma once

#include <string>
#include <cstdint>

namespace lazarus
{
namespace storage
{

class collocation_resolver
{
public:

    //
    // Constructor.
    //
    collocation_resolver(
        const std::uint16_t number_data_collocations);

    //
    // Returns the appropriate collocation
    // index to be used for the object key.
    //
    std::uint16_t
    get_collocation_index_for_key(
        const std::string& object_key);

private:

    //
    // Hasher for routing an object key to its respective data collocation.
    //
    std::hash<std::string> hasher_;

    //
    // Number of data collocations in the system.
    //
    const std::uint16_t number_data_collocations_;
};

} // namespace storage.
} // namespace lazarus.