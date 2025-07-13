// ****************************************************
// Lazarus Data Store
// Storage
// 'concurrent_io_dispatcher.cc'
// Author: jcjuarez
// Description:
//      Thread pool based dispatcher
//      for storage IO operations.
// ****************************************************

#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "container.hh"
#include "concurrent_io_dispatcher.hh"

namespace lazarus::storage
{

concurrent_io_dispatcher::concurrent_io_dispatcher(
    const std::uint32_t number_threads,
    std::shared_ptr<storage_engine> storage_engine)
    : storage_engine_{std::move(storage_engine)},
      io_thread_pool_{number_threads}
{}

void
concurrent_io_dispatcher::enqueue_concurrent_io_request(
    schemas::object_request_interface&& object_request,
    std::shared_ptr<container> object_container,
    network::server_response_callback&& response_callback)
{
    //
    // Enqueue the async IO action.
    // This is a concurrent operation.
    //
    boost::asio::post(io_thread_pool_,
        [this,
          object_request = std::move(object_request),
          object_container = std::move(object_container),
          response_callback = std::move(response_callback)]() mutable
        {
          this->concurrent_io_request_proxy(
              std::move(object_request),
              object_container,
              std::move(response_callback));
        });
}

} // namespace lazarus::common.