// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'io_dispatcher_interface.hh'
// Author: jcjuarez
// Description:
//      Dispatcher interface for IO operations.
// ****************************************************

#pragma once

#include "../common/interface.hh"
#include "../schemas/request-interfaces/object_request.hh"

namespace lazarus
{
namespace storage
{

class io_dispatcher_interface : common::interface
{
public:

    //
    // Enqueues an IO operation for to be processed by the dispatcher.
    // Implementers must provide a response back to the server through the async callback.
    //
    virtual
    void
    enqueue_io_task(
        schemas::object_request&& object_request,
        std::shared_ptr<container> container,
        network::server_response_callback&& response_callback) = 0;

    //
    // Waits for the dispatcher to be stopped in a blocking manner.
    //
    virtual
    void
    wait_for_stop() = 0;
};

} // namespace storage.
} // namespace lazarus.