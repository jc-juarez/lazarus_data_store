// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'read_io_dispatcher.hh'
// Author: jcjuarez
// Description:
//      Read request dispatcher module for IO.
//      Implemented as a thread pool dispatcher.
// ****************************************************

#include "../models/container.hh"
#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "../cache/frontline_cache.hh"
#include "read_io_dispatcher.hh"

namespace lazarus::storage
{

read_io_dispatcher::read_io_dispatcher(
    const std::uint32_t number_read_io_threads,
    std::shared_ptr<storage_engine_interface> storage_engine,
    std::shared_ptr<frontline_cache> frontline_cache)
    : storage_engine_{std::move(storage_engine)},
      frontline_cache_{std::move(frontline_cache)},
      read_io_thread_pool_{number_read_io_threads}
{}

void
read_io_dispatcher::enqueue_io_task(
    object_io_task&& object_io_task)
{
    //
    // Enqueue the async IO action.
    // This is a concurrent operation.
    //
    boost::asio::post(read_io_thread_pool_,
        [this,
        object_io_task = std::move(object_io_task)]() mutable
        {
          this->dispatch_read_io_task(
              std::move(object_io_task));
        });
}

void
read_io_dispatcher::wait_for_stop()
{
    //
    // This call will make sure that all queued and in-progress
    // tasks within the thread pool are completed, in a blocking manner.
    //
    read_io_thread_pool_.join();
}

void
read_io_dispatcher::dispatch_read_io_task(
    object_io_task&& object_io_task)
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
    switch (object_io_task.object_request_.get_optype())
    {
        case schemas::object_request_optype::get:
        {
            status = execute_get_operation(
                object_io_task.container_->get_storage_engine_reference(),
                object_io_task.object_request_,
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
                static_cast<std::uint8_t>(object_io_task.object_request_.get_optype()),
                object_io_task.object_request_.get_object_id(),
                object_io_task.object_request_.get_container_name());

            status = status::invalid_operation;
            break;
        }
    }

    if (status::succeeded(status))
    {
        //
        // Only send the response fields if the request succeeded.
        //
        network::response_fields response_fields;
        response_fields.emplace(schemas::object_request::object_data_key_tag, &object_data);
        network::server::send_response(
            object_io_task.response_callback_,
            status,
            &response_fields);

        //
        // If the operation was successful, insert the object into the cache,
        // but only after replying back to the server. Cache insertions triggered
        // by get operations do not need a strong feedback loop; eventual cache alignment is accepted.
        //
        insert_object_into_cache(object_io_task.object_request_);
    }
    else
    {
        //
        // Empty response on failure.
        //
        network::server::send_response(
            object_io_task.response_callback_,
            status);
    }
}

void
read_io_dispatcher::insert_object_into_cache(
    schemas::object_request& object_request)
{
    //
    // Create a copy of these parameters since
    // they will be moved after the cache insertion.
    //
    const std::string object_id = object_request.get_object_id();
    const std::string container_name = object_request.get_container_name();
    const status::status_code status = frontline_cache_->put(
        std::move(object_request.get_object_id_mutable()),
        std::move(object_request.get_object_data_mutable()),
        std::move(object_request.get_container_name_mutable()));

    if (status::succeeded(status))
    {
        spdlog::info("Frontline cache object insertion succeeded on get object operation. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_id,
            container_name);
    }
    else
    {
        spdlog::error("Frontline cache object insertion failed on get object operation. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_id,
            container_name,
            status);
    }
}

status::status_code
read_io_dispatcher::execute_get_operation(
    storage_engine_reference_handle* container_storage_engine_reference,
    const schemas::object_request& object_request,
    byte_stream& object_data)
{
    status::status_code status = storage_engine_->get_object(
        container_storage_engine_reference,
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
            object_request.get_container_name());
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
            object_request.get_container_name(),
            status);
    }

    return status;
}

} // namespace lazarus::common.