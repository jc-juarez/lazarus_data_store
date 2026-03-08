// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'threading_context_table.cc'
// Author: jcjuarez
// Description:
//      Contains all threading contexts in the system.
// ****************************************************

#include "threading_context.hh"
#include "threading_context_table.hh"

namespace lazarus
{
namespace storage
{

threading_context_table::threading_context_table()
{}

void
threading_context_table::append_context(
    const std::uint16_t collocation_index)
{
    threading_contexts_.emplace_back(
        std::make_shared<threading_context>(
            collocation_index));
}

std::shared_ptr<threading_context>
threading_context_table::get_threading_context(
    const std::uint16_t collocation_index)
{
    return threading_contexts_.at(collocation_index);
}

std::vector<std::shared_ptr<threading_context>>
threading_context_table::get_all_contexts()
{
    return threading_contexts_;
}

} // namespace storage.
} // namespace lazarus.