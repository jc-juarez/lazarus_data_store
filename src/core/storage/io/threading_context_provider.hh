// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'threading_context_provider.hh'
// Author: jcjuarez
// Description:
//      Provides access to specific threading contexts.
// ****************************************************

#pragma once

#include <vector>
#include <memory>
#include "threading_context_table.hh"

namespace lazarus
{
namespace storage
{

class threading_context;
class collocation_resolver;

class threading_context_provider
{
public:

    //
    // Constructor.
    //
    threading_context_provider(
        std::unique_ptr<threading_context_table> threading_context_table);

    //
    // Provides access to a specific data partition by collocation index lookup.
    //
    std::shared_ptr<threading_context>
    get_context_by_collocation(
        const std::uint16_t collocation_index);

    //
    // Returns a list with all data partitions in the system.
    //
    std::vector<std::shared_ptr<threading_context>>
    get_all_contexts();

private:

    //
    // Table containing all threading contexts in the system.
    //
    std::unique_ptr<threading_context_table> contexts_table_;
};

} // namespace storage.
} // namespace lazarus.