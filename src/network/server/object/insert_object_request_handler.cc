// ****************************************************
// Lazarus Data Store
// Network
// 'insert_object_request_handler.cc'
// Author: jcjuarez
// Description:
//      Manages the top-level orchestration for
//      object insertion requests.
// ****************************************************

#include "../server.hh"
#include <spdlog/spdlog.h>
#include "insert_object_request_handler.hh"
#include "../../../storage/object_management_service.hh"

namespace lazarus
{
namespace network
{

insert_object_request_handler::insert_object_request_handler(
    std::shared_ptr<storage::object_management_service> object_management_service)
    : object_request_handler{std::move(object_management_service)}
{}

void
insert_object_request_handler::execute_operation(
    schemas::object_request&& object_request,
    std::shared_ptr<storage::container> container,
    server_response_callback&& response_callback)
{
    spdlog::info("Insert object request received. "
        "Optype={}, "
        "ObjectId={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(object_request.get_optype()),
        object_request.get_object_id(),
        object_request.get_container_name());

    //
    // At this point, it is guaranteed that ref-counted reference
    // to the object container is held, so enqueue a concurrent write
    // operation. The response will be provided asynchronously over the
    // provided callback if the enqueue operation is successful.
    //
    status::status_code status = object_management_service_->orchestrate_concurrent_write_request(
        std::move(object_request),
        container,
        std::move(response_callback));

    if (status::failed(status))
    {
        spdlog::error("Failed to enqueue object insert request operation. "
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
