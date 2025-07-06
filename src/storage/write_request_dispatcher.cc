// ****************************************************
// Lazarus Data Store
// Storage
// 'write_request_dispatcher.hh'
// Author: jcjuarez
// Description:
//      Write request dispatcher module for IO.
//      Implemented as a thread pool dispatcher.
// ****************************************************

#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "object_container.hh"
#include "write_request_dispatcher.hh"

namespace lazarus::storage
{

write_request_dispatcher::write_request_dispatcher(
    const std::uint32_t number_write_io_threads,
    std::shared_ptr<storage_engine> storage_engine)
    : storage_engine_{std::move(storage_engine)},
      write_io_thread_pool_{static_cast<std::int32_t>(number_write_io_threads)}
{}

void
write_request_dispatcher::enqueue_write_request(
    schemas::object_request_interface&& object_request,
    std::shared_ptr<object_container> object_container,
    network::server_response_callback&& response_callback)
{
    //
    // Enqueue the async write IO action.
    // This is a concurrent operation.
    //
    write_io_thread_pool_.execute(
        [this,
        object_request = std::move(object_request),
        object_container = std::move(object_container),
        response_callback = std::move(response_callback)]() mutable
        {
            this->write_request_concurrent_proxy(
                std::move(object_request),
                object_container,
                std::move(response_callback));
        });
}

void
write_request_dispatcher::write_request_concurrent_proxy(
    schemas::object_request_interface&& object_request,
    std::shared_ptr<object_container> object_container,
    network::server_response_callback&& response_callback)
{
    //
    // The underlying storage engine is of blocking nature,
    // so all threads reaching the storage engine API will block until
    // the storage IO makes progress, as to later provide a response to the client.
    //
    status::status_code status = status::success;

    //
    // At this point of execution, this scope guarantees that the
    // object container will be held for as long as the storage API takes,
    // so it is safe to only pass down the storage engine reference given
    // the storage engine reference will not be dropped by the storage engine.
    //
    switch (object_request.get_optype())
    {
        case schemas::object_request_optype::insert:
        {
            status = execute_insert_operation(
                object_container->get_storage_engine_reference(),
                object_request);
            break;
        }
        case schemas::object_request_optype::remove:
        {
            status = execute_remove_operation(
                object_container->get_storage_engine_reference(),
                object_request);
            break;
        }
        default:
        {
            //
            // This should never happen given this should have been
            // taken care of before enqueuing the task to the thread pool.
            // Log the issue and assert.
            //
            spdlog::critical("Invalid write request optype for object "
                "operation scheduled in the write IO thread pool. "
                "Optype={}, "
                "ObjectId={}, "
                "ObjectContainerName={}.",
                static_cast<std::uint8_t>(object_request.get_optype()),
                object_request.get_object_id(),
                object_request.get_object_container_name());

            spdlog::shutdown();
            assert(false);
            std::abort();
        }
    }

    //
    // Provide the response back to the client over the async callback.
    //
    network::server::send_response(
        response_callback,
        status);
}

status::status_code
write_request_dispatcher::execute_insert_operation(
    storage_engine_reference_handle* object_container_storage_engine_reference,
    const schemas::object_request_interface& object_request)
{
    return storage_engine_->insert_object(
        object_container_storage_engine_reference,
        object_request.get_object_id().c_str(),
        object_request.get_object_data());
}

status::status_code
write_request_dispatcher::execute_remove_operation(
    storage_engine_reference_handle* object_container_storage_engine_reference,
    const schemas::object_request_interface& object_request)
{
    return storage_engine_->remove_object(
        object_container_storage_engine_reference,
        object_request.get_object_id().c_str());
}

} // namespace lazarus::common.