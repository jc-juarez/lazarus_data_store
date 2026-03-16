// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'threading_context_table.hh'
// Author: jcjuarez
// Description:
//      Contains all threading contexts in the system.
// ****************************************************

#pragma once

#include <vector>
#include <memory>
#include "threading_context.hh"

namespace pandora
{
namespace storage
{

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
        const std::uint16_t collocation_index,
        std::unique_ptr<io_dispatcher_interface> read_io_dispatcher,
        std::unique_ptr<io_dispatcher_interface> write_io_dispatcher);

    //
    // Returns a reference to a threading context.
    // This API is meant to be only be consumed by the threading context provider.
    //
    threading_context&
    get_threading_context(
        const std::uint16_t collocation_index);

    //
    // Starts all write IO dispatcher threads.
    //
    void
    start_write_io_dispatching();

    //
    // Gets the total number of threading contexts.
    //
    std::uint16_t
    get_num_contexts();

    //
    // Gets the total number of read IO threads across all contexts.
    //
    std::uint32_t
    get_num_read_io_threads();

    //
    // Gets the total number of write IO threads across all contexts.
    //
    std::uint32_t
    get_num_write_io_threads();

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
    std::vector<threading_context> threading_contexts_;
};

} // namespace storage.
} // namespace pandora.