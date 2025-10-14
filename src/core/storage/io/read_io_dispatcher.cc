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

#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "read_io_dispatcher.hh"
#include "object_io_executor.hh"
#include "../cache/cache_accessor.hh"

namespace lazarus::storage
{

read_io_dispatcher::read_io_dispatcher(
    const std::uint32_t number_read_io_threads,
    std::shared_ptr<object_io_executor> object_io_executor,
    std::shared_ptr<cache_accessor> cache_accessor)
    : object_io_executor_{std::move(object_io_executor)},
      cache_accessor_{std::move(cache_accessor)},
      read_io_thread_pool_{number_read_io_threads}
{}

void
read_io_dispatcher::enqueue_io_task(
    object_io_task&& read_io_task)
{
    //
    // Enqueue the async IO action.
    // This is a concurrent operation.
    //
    boost::asio::post(read_io_thread_pool_,
        [this,
        read_io_task = std::move(read_io_task)]() mutable
        {
          this->dispatch_read_io_task(
              std::move(read_io_task));
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
    object_io_task&& read_io_task)
{
    //
    // At this point of execution, this scope guarantees that the
    // object container will be held for as long as the storage API takes,
    // so it is safe to only pass down the storage engine reference given
    // the storage engine reference will not be dropped by the storage engine.
    //
    byte_stream object_data;
    status::status_code status = execute_read_io_task(
        read_io_task,
        object_data);

    if (status::failed(status))
    {
        //
        // Empty response on failure.
        //
        network::server::send_response(
            read_io_task.response_callback_,
            status);

        return;
    }

    //
    // On success, send the response fields with the object data.
    //
    network::response_fields response_fields;
    response_fields.emplace(schemas::object_request::object_data_key_tag, &object_data);
    network::server::send_response(
        read_io_task.response_callback_,
        status,
        &response_fields);

    //
    // If the operation was successful, insert the object into the cache,
    // but only after replying back to the server. Cache insertions triggered
    // by get operations do not need a strong feedback loop; eventual cache alignment is accepted.
    //
    cache_accessor_->insert_object_into_cache(
        read_io_task.object_request_);
}

status::status_code
read_io_dispatcher::execute_read_io_task(
    object_io_task& read_io_task,
    byte_stream& object_data)
{
    //
    // The underlying storage engine is of blocking nature,
    // so all threads reaching the storage engine API will block until
    // the storage IO makes progress, as to later provide a response to the client.
    //
    switch (read_io_task.object_request_.get_optype())
    {
        case schemas::object_request_optype::get:
        {
            return object_io_executor_->execute_get_operation(
                read_io_task.container_->get_storage_engine_reference(),
                read_io_task.object_request_,
                object_data);
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
                static_cast<std::uint8_t>(read_io_task.object_request_.get_optype()),
                read_io_task.object_request_.get_object_id(),
                read_io_task.object_request_.get_container_name());

            return status::invalid_operation;
        }
    }
}

} // namespace lazarus::storage.