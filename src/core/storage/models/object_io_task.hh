// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'object_io_task.hh'
// Author: jcjuarez
// Description:
//      Individual unit of data
//      for object IO operations.
//      Members are intended to be of free use.
// ****************************************************

#pragma once

#include "container.hh"
#include "../../common/aliases.hh"
#include "../../common/uuid_utilities.hh"
#include "../../schemas/request-interfaces/object_request.hh"

namespace pandora::storage
{

class object_io_task
{
public:

    //
    // Constructor with moved parameters.
    //
    object_io_task(
        const std::uint16_t collocation_index,
        schemas::object_request&& object_request,
        std::shared_ptr<container> container,
        network::server_response_callback&& response_callback);

    //
    // Move constructor.
    //
    object_io_task(
        object_io_task&& other);

    //
    // Contains the data for the object request.
    //
    schemas::object_request object_request_;

    //
    // Alive reference for the container engine.
    //
    std::shared_ptr<container> container_;

    //
    // Callback on which to provide a response for
    // the operation associated to the respective IO task.
    //
    network::server_response_callback response_callback_;

    //
    // Represents the collocation index corresponding for the operation.
    //
    const std::uint16_t collocation_index_;

private:

    //
    // Unique ID for the object task.
    // Generated upon construction.
    //
    const boost::uuids::uuid id_;
};

} // namespace pandora::storage.