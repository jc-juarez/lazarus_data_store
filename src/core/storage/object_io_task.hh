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
// ****************************************************

#pragma once

#include "container.hh"
#include "../common/aliases.hh"
#include "../schemas/request-interfaces/object_request.hh"

namespace lazarus::storage
{

struct object_io_task
{
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