// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'threading_context_table.hh'
// Author: jcjuarez
// Description:
//      Contains all threading contexts in the system.
// ****************************************************

#pragma once

#include <vector>
#include <memory>

namespace lazarus
{
namespace storage
{

class threading_context;

class threading_context_table
{
public:

    //
    // Constructor.
    //
    threading_context_table();

    //
    // Appends a new threading context instance to the table.
    //
    void
    append_context(
        const std::uint16_t collocation_index);

    //
    // Returns a reference to a threading context.
    // This API is meant to be only be consumed by the threading context provider.
    //
    std::shared_ptr<threading_context>
    get_threading_context(
        const std::uint16_t collocation_index);

    //
    // Returns a list with all threading contexts in the system.
    //
    std::vector<std::shared_ptr<threading_context>>
    get_all_contexts();

private:

    //
    // Table for holding all threading contexts in an owning model.
    // References to be provided should be owning.
    //
    // The table functions as a direct mapping given a collocation index:
    // --------------------------------------------------------------
    // | Offset_0 | Offset_1 | Offset_2 | Offset_3 | ... | Offset_N |
    // --------------------------------------------------------------
    // |   TC_0   |   TC_1   |   TC_2   |   TC_3   | ... |   TC_N   |
    // --------------------------------------------------------------
    //
    std::vector<std::shared_ptr<threading_context>> threading_contexts_;
};

} // namespace storage.
} // namespace lazarus.