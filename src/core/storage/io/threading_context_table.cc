// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'threading_context_table.cc'
// Author: jcjuarez
// Description:
//      Contains all threading contexts in the system.
// ****************************************************

#include "threading_context.hh"
#include "threading_context_table.hh"
#include "io_dispatcher_interface.hh"
#include "../../common/startable_interface.hh"

namespace pandora
{
namespace storage
{

threading_context_table::threading_context_table()
{}

void
threading_context_table::append_context(
    const std::uint16_t collocation_index,
    std::unique_ptr<io_dispatcher_interface> read_io_dispatcher,
    std::unique_ptr<io_dispatcher_interface> write_io_dispatcher)
{
    threading_contexts_.emplace_back(
            collocation_index,
            std::move(read_io_dispatcher),
            std::move(write_io_dispatcher));
}

threading_context&
threading_context_table::get_threading_context(
    const std::uint16_t collocation_index)
{
    return threading_contexts_.at(collocation_index);
}

void
threading_context_table::start_write_io_dispatching()
{
    for (auto& threading_context : threading_contexts_)
    {
        auto write_io_dispatcher = dynamic_cast<common::startable_interface*>(
            &(threading_context.get_write_io_dispatcher()));
        write_io_dispatcher->start();
    }
}

std::uint16_t
threading_context_table::get_num_contexts()
{
    return threading_contexts_.size();
}

std::uint32_t
threading_context_table::get_num_read_io_threads()
{
    std::uint32_t num_read_io_threads = 0u;
    for (auto& context : threading_contexts_)
    {
        num_read_io_threads += context.get_num_read_io_threads();
    }
    return num_read_io_threads;
}

std::uint32_t
threading_context_table::get_num_write_io_threads()
{
    std::uint32_t num_write_io_threads = 0u;
    for (auto& context : threading_contexts_)
    {
        num_write_io_threads += context.get_num_write_io_threads();
    }
    return num_write_io_threads;
}

} // namespace storage.
} // namespace pandora.