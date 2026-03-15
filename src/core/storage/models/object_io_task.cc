// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'object_io_task.cc'
// Author: jcjuarez
// Description:
//      Individual unit of data
//      for object IO operations.
//      Members are intended to be of free use.
// ****************************************************

#include "object_io_task.hh"

namespace pandora::storage
{

object_io_task::object_io_task(
    const std::uint16_t collocation_index,
    schemas::object_request&& object_request,
    std::shared_ptr<container> container,
    network::server_response_callback&& response_callback)
    : collocation_index_{collocation_index},
      object_request_{std::move(object_request)},
      container_{std::move(container)},
      response_callback_{std::move(response_callback)},
      id_{common::generate_uuid()}
{}

object_io_task::object_io_task(
    object_io_task&& other)
    : collocation_index_{other.collocation_index_},
      object_request_{std::move(other.object_request_)},
      container_{std::move(other.container_)},
      response_callback_{std::move(other.response_callback_)},
      id_{std::move(other.id_)}
{}

} // namespace pandora::storage.