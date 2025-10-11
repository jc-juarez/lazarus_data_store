// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'write_io_task.hh'
// Author: jcjuarez
// Description:
//      Individual unit of data
//      for object IO operations.
//      Members are intended to be of free use.
// ****************************************************

#pragma once

#include "container.hh"
#include "../../common/aliases.hh"
#include "../../schemas/request-interfaces/object_request.hh"

namespace lazarus::storage
{

struct object_io_task
{
    //
    // Inline constructor with moved parameters.
    //
    object_io_task(
        schemas::object_request&& object_request,
        std::shared_ptr<container> container,
        network::server_response_callback&& response_callback)
        : object_request_{std::move(object_request)},
          container_{std::move(container)},
          response_callback_{std::move(response_callback)}
    {}

    //
    // Inline move constructor.
    //
    object_io_task(
        object_io_task&& other)
        : object_request_{std::move(other.object_request_)},
          container_{std::move(other.container_)},
          response_callback_{std::move(other.response_callback_)}
    {}

    //
    // Contains the data for the object request.
    //
    schemas::object_request object_request_;

    //
    // Alive reference for the container engine.
    //
    std::shared_ptr<container> container_;

    //
    // Callback on which to provide a response for the operation.
    //
    network::server_response_callback response_callback_;
};

} // namespace lazarus::storage.