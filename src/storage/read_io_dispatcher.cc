// ****************************************************
// Lazarus Data Store
// Storage
// 'read_io_dispatcher.hh'
// Author: jcjuarez
// Description:
//      Read request dispatcher module for IO.
//      Implemented as a thread pool dispatcher.
// ****************************************************

#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "object_container.hh"
#include "read_io_dispatcher.hh"

namespace lazarus::storage
{

read_io_dispatcher::read_io_dispatcher(
    const std::uint32_t number_read_io_threads,
    std::shared_ptr<storage_engine> storage_engine)
    : concurrent_io_dispatcher{
    number_read_io_threads,
    std::move(storage_engine)}
{}

void
read_io_dispatcher::concurrent_io_request_proxy(
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
    byte_stream object_data;
    switch (object_request.get_optype())
    {
        case schemas::object_request_optype::get:
        {
            status = execute_get_operation(
                object_container->get_storage_engine_reference(),
                object_request,
                object_data);
            break;
        }
        default:
        {
            //
            // This should never happen given this should have been
            // taken care of before enqueuing the task to the thread pool.
            //
            spdlog::critical("Invalid read request optype for object "
                "operation scheduled in the read IO thread pool. "
                "Optype={}, "
                "ObjectId={}, "
                "ObjectContainerName={}.",
                static_cast<std::uint8_t>(object_request.get_optype()),
                object_request.get_object_id(),
                object_request.get_object_container_name());

            status = status::invalid_operation;
            break;
        }
    }

    if (status::succeeded(status))
    {
        //
        // Only send the response fields if the request succeeded.
        //
        network::response_fields response_fields
            {{schemas::object_request_interface::object_data_key_tag, std::move(object_data)}};

        network::server::send_response(
            response_callback,
            status,
            &response_fields);
    }
    else
    {
        //
        // Empty response on failure.
        //
        network::server::send_response(
            response_callback,
            status);
    }
}

status::status_code
read_io_dispatcher::execute_get_operation(
    storage_engine_reference_handle* object_container_storage_engine_reference,
    const schemas::object_request_interface& object_request,
    byte_stream& object_data)
{
    status::status_code status = storage_engine_->get_object(
        object_container_storage_engine_reference,
        object_request.get_object_id().c_str(),
        &object_data);

    if (status::succeeded(status))
    {
        spdlog::info("Object retrieval succeeded. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_object_container_name());
    }
    else
    {
        spdlog::error("Object retrieval failed. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_object_container_name(),
            status);
    }

    return status;
}

} // namespace lazarus::common.