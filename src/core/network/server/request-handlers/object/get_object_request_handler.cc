// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Network
// 'get_object_request_handler.cc'
// Author: jcjuarez
// Description:
//      Manages the top-level orchestration for
//      object retrieval requests.
// ****************************************************

#include "../../server.hh"
#include <spdlog/spdlog.h>
#include "get_object_request_handler.hh"
#include "../../../../storage/object_management_service.hh"

namespace lazarus
{
namespace network
{

get_object_request_handler::get_object_request_handler(
    std::shared_ptr<storage::object_management_service> object_management_service)
    : object_request_handler{std::move(object_management_service)}
{}

void
get_object_request_handler::execute_operation(
    schemas::object_request&& object_request,
    std::shared_ptr<storage::container> container,
    server_response_callback&& response_callback)
{
    spdlog::info("Read object request received. "
        "Optype={}, "
        "ObjectId={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(object_request.get_optype()),
        object_request.get_object_id(),
        object_request.get_container_name());

    //
    // Before calling the thread pool to look for the object,
    // check the frontline cache. If present, this avoids
    // context-switch overhead and calling the storage engine backend.
    //
    std::optional<storage::byte_stream> object_data = object_management_service_->get_object_from_frontline_cache(
        object_request.get_object_id(),
        object_request.get_container_name());

    if (object_data.has_value())
    {
        spdlog::info("Frontline cache has the requested object for get object operation. Completing request. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_container_name());

        network::response_fields response_fields;
        response_fields.emplace(schemas::object_request::object_data_key_tag, &(object_data.value()));
        network::server::send_response(
            response_callback,
            status::success,
            &response_fields);

        return;
    }

    //
    // At this point, it is guaranteed that ref-counted reference
    // to the object container is held, so enqueue a concurrent write
    // operation. The response will be provided asynchronously over the
    // provided callback if the enqueue operation is successful.
    //
    status::status_code status = object_management_service_->orchestrate_concurrent_read_request(
        std::move(object_request),
        container,
        std::move(response_callback));

    if (status::failed(status))
    {
        spdlog::error("Failed to enqueue object get request operation. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_container_name(),
            status);

        network::server::send_response(
            response_callback,
            status);
    }
}

} // namespace network.
} // namespace lazarus.
