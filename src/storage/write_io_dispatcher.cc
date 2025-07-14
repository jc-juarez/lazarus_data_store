// ****************************************************
// Lazarus Data Store
// Storage
// 'write_io_dispatcher.cc'
// Author: jcjuarez
// Description:
//      Write request dispatcher module for IO.
//      Implemented as a thread pool dispatcher.
// ****************************************************

#include "container.hh"
#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "frontline_cache.hh"
#include "write_io_dispatcher.hh"

namespace lazarus::storage
{

write_io_dispatcher::write_io_dispatcher(
    const std::uint32_t number_write_io_threads,
    std::shared_ptr<storage_engine> storage_engine,
    std::shared_ptr<storage::frontline_cache> frontline_cache)
    : concurrent_io_dispatcher{
        number_write_io_threads,
        std::move(storage_engine),
        std::move(frontline_cache)}
{}

void
write_io_dispatcher::concurrent_io_request_proxy(
    schemas::object_request&& object_request,
    std::shared_ptr<container> container,
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
                container->get_storage_engine_reference(),
                object_request);
            break;
        }
        case schemas::object_request_optype::remove:
        {
            status = execute_remove_operation(
                container->get_storage_engine_reference(),
                object_request);
            break;
        }
        default:
        {
            //
            // This should never happen given this should have been
            // taken care of before enqueuing the task to the thread pool.
            //
            spdlog::critical("Invalid write request optype for object "
                "operation scheduled in the write IO thread pool. "
                "Optype={}, "
                "ObjectId={}, "
                "ObjectContainerName={}.",
                static_cast<std::uint8_t>(object_request.get_optype()),
                object_request.get_object_id(),
                object_request.get_container_name());

            status = status::invalid_operation;
            break;
        }
    }

    //
    // Provide the response back to the client over the async callback.
    //
    network::server::send_response(
        response_callback,
        status);

    //
    // If the operation was successful, insert the object into the cache,
    // but only after replying back to the server. The cache is not intended to be
    // strongly consistent, so clients might not see the new object entry until later.
    //
}

status::status_code
write_io_dispatcher::execute_insert_operation(
    storage_engine_reference_handle* container_storage_engine_reference,
    const schemas::object_request& object_request)
{
    status::status_code status = storage_engine_->insert_object(
        container_storage_engine_reference,
        object_request.get_object_id().c_str(),
        object_request.get_object_data());

    if (status::succeeded(status))
    {
        spdlog::info("Object insertion succeeded. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_container_name());
    }
    else
    {
        spdlog::error("Object insertion failed. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_container_name(),
            status);
    }

    return status;
}

status::status_code
write_io_dispatcher::execute_remove_operation(
    storage_engine_reference_handle* container_storage_engine_reference,
    const schemas::object_request& object_request)
{
    status::status_code status = storage_engine_->remove_object(
        container_storage_engine_reference,
        object_request.get_object_id().c_str());

    if (status::succeeded(status))
    {
        spdlog::info("Object removal succeeded. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_container_name());
    }
    else
    {
        spdlog::error("Object removal failed. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_container_name(),
            status);
    }

    return status;
}

} // namespace lazarus::common.