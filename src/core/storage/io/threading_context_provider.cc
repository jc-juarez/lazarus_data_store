// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'threading_context_provider.cc'
// Author: jcjuarez
// Description:
//      Provides access to specific threading contexts.
// ****************************************************

#include "threading_context.hh"
#include "threading_context_table.hh"
#include "threading_context_provider.hh"

namespace lazarus
{
namespace storage
{

threading_context_provider::threading_context_provider(
    std::unique_ptr<threading_context_table> threading_context_table)
    : contexts_table_{std::move(threading_context_table)}
{}

std::shared_ptr<threading_context>
threading_context_provider::get_context_by_collocation(
    const std::uint16_t collocation_index)
{
    return contexts_table_->get_threading_context(collocation_index);
}

std::vector<std::shared_ptr<threading_context>>
threading_context_provider::get_all_contexts()
{
    return contexts_table_->get_all_contexts();
}

} // namespace storage.
} // namespace lazarus.