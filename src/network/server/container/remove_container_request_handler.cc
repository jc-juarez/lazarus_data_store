// ****************************************************
// Lazarus Data Store
// Network
// 'remove_container_request_handler.cc'
// Author: jcjuarez
// Description:
//      Manages the top-level orchestration for
//      container removal requests.
// ****************************************************

#include "../server.hh"
#include <spdlog/spdlog.h>
#include "remove_container_request_handler.hh"
#include "../../../storage/container_management_service.hh"

namespace lazarus
{
namespace network
{

remove_container_request_handler::remove_container_request_handler(
    std::shared_ptr<storage::container_management_service> container_management_service)
    : container_request_handler{std::move(container_management_service)}
{}

void
remove_container_request_handler::execute_operation(
    schemas::container_request&& container_request,
    server_response_callback&& response_callback)
{
    spdlog::info("Remove object container request received. "
        "Optype={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(container_request.get_optype()),
        container_request.get_name());

    //
    // Orchestrate the removal of the object
    // container in async serialized fashion.
    // Response will be provided by a separate thread.
    //
    container_management_service_->orchestrate_serial_container_operation(
        std::move(container_request),
        std::move(response_callback));
}

} // namespace network.
} // namespace lazarus.