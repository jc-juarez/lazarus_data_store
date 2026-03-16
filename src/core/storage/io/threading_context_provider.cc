// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'threading_context_provider.cc'
// Author: jcjuarez
// Description:
//      Provides access to specific threading contexts.
// ****************************************************

#include "threading_context.hh"
#include "io_dispatcher_interface.hh"
#include "threading_context_table.hh"
#include "threading_context_provider.hh"

namespace pandora
{
namespace storage
{

threading_context_provider::threading_context_provider(
    std::unique_ptr<threading_context_table> threading_context_table)
    : contexts_table_{std::move(threading_context_table)}
{}

threading_context&
threading_context_provider::get_context_by_collocation(
    const std::uint16_t collocation_index)
{
    return contexts_table_->get_threading_context(collocation_index);
}

void
threading_context_provider::start_write_io_dispatching()
{
    contexts_table_->start_write_io_dispatching();
}

std::uint16_t
threading_context_provider::get_num_contexts()
{
    return contexts_table_->get_num_contexts();
}

std::uint32_t
threading_context_provider::get_num_read_io_threads()
{
    return contexts_table_->get_num_read_io_threads();
}

std::uint32_t
threading_context_provider::get_num_write_io_threads()
{
    return contexts_table_->get_num_write_io_threads();
}

} // namespace storage.
} // namespace pandora.