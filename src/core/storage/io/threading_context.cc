// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'threading_context.cc'
// Author: jcjuarez
// Description:
//      Scheduling context for executing object IO
//      operations.
// ****************************************************

#include "threading_context.hh"
#include "io_dispatcher_interface.hh"

namespace pandora
{
namespace storage
{

threading_context::threading_context(
    const std::uint16_t collocation_index,
    std::unique_ptr<io_dispatcher_interface> read_io_dispatcher,
    std::unique_ptr<io_dispatcher_interface> write_io_dispatcher)
    : collocation_index_{collocation_index},
      read_io_dispatcher_{std::move(read_io_dispatcher)},
      write_io_dispatcher_{std::move(write_io_dispatcher)}
{}

io_dispatcher_interface&
threading_context::get_read_io_dispatcher()
{
    return *(read_io_dispatcher_.get());
}

io_dispatcher_interface&
threading_context::get_write_io_dispatcher()
{
    return *(write_io_dispatcher_.get());
}

std::uint16_t
threading_context::get_num_write_io_threads()
{
    return write_io_dispatcher_->get_num_io_threads();
}

std::uint16_t
threading_context::get_num_read_io_threads()
{
    return read_io_dispatcher_->get_num_io_threads();
}

} // namespace storage.
} // namespace pandora.