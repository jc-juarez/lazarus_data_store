// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'threading_context.hh'
// Author: jcjuarez
// Description:
//      Scheduling context for executing object IO
//      operations.
// ****************************************************

#pragma once

#include <memory>
#include <cstdint>

namespace pandora
{
namespace storage
{

class io_dispatcher_interface;

class threading_context
{
public:

    //
    // Constructor.
    //
    threading_context(
        const std::uint16_t collocation_index,
        std::unique_ptr<io_dispatcher_interface> read_io_dispatcher,
        std::unique_ptr<io_dispatcher_interface> write_io_dispatcher);

    //
    // Provides access to the underlying read IO dispatcher.
    //
    io_dispatcher_interface&
    get_read_io_dispatcher();

    //
    // Provides access to the underlying write IO dispatcher.
    //
    io_dispatcher_interface&
    get_write_io_dispatcher();

    //
    // Gets the total number of write IO threads.
    //
    std::uint16_t
    get_num_write_io_threads();

    //
    // Gets the total number of read IO threads.
    //
    std::uint16_t
    get_num_read_io_threads();

private:

    //
    // Corresponding data collocation index.
    //
    const std::uint16_t collocation_index_;

    //
    // Read IO dispatcher.
    //
    std::unique_ptr<io_dispatcher_interface> read_io_dispatcher_;

    //
    // Write IO dispatcher.
    //
    std::unique_ptr<io_dispatcher_interface> write_io_dispatcher_;
};

} // namespace storage.
} // namespace pandora.