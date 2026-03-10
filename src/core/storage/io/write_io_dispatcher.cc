// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'write_io_dispatcher.cc'
// Author: jcjuarez
// Description:
//      Write request dispatcher module for IO.
//      Implemented as a single-threaded lock-free
//      batching IO dispatcher.
// ****************************************************

#include <spdlog/spdlog.h>
#include "write_io_dispatcher.hh"
#include "../../common/aliases.hh"
#include "../cache/cache_accessor.hh"
#include "data_partition_provider.hh"
#include "storage_engine_interface.hh"
#include "../../startup/system_init.hh"
#include "../../network/server/server.hh"

namespace lazarus::storage
{

write_io_dispatcher::write_io_dispatcher(
    data_partition_provider& data_partition_provider,
    cache_accessor& cache_accessor)
    : data_partition_provider_{data_partition_provider},
      cache_accessor_{cache_accessor}
{}

void
write_io_dispatcher::start()
{
    spdlog::info("Starting lazarus data store write IO dispatcher thread.");

    write_dispatcher_master_thread_ = std::jthread(
        &write_io_dispatcher::dispatch_write_io_tasks,
        this,
        get_stop_source_token());
}

void
write_io_dispatcher::enqueue_io_task(
    object_io_task&& write_io_task)
{
    //
    // This is a lock-free operation.
    //
    write_io_tasks_queue_.enqueue(
        std::make_unique<object_io_task>(
            std::move(write_io_task)));
}

void
write_io_dispatcher::wait_for_stop()
{
    //
    // This call will make sure that the dispatcher
    // thread gets stopped, in a blocking manner.
    //
    write_dispatcher_master_thread_.join();
}

void
write_io_dispatcher::dispatch_write_io_tasks(
    std::stop_token stop_token)
{
    //
    // This is the core tight loop for dispatching write io tasks.
    //
    while (!stop_token.stop_requested())
    {
        std::unique_ptr<object_io_task> write_io_task;
        if (write_io_tasks_queue_.wait_dequeue_timed(write_io_task, std::chrono::seconds(1u)))
        {
            execute_write_io_task(std::move(write_io_task));
        }
    }

    spdlog::info("Stopping lazarus data store write IO dispatcher thread.");
}

void
write_io_dispatcher::execute_write_io_task(
    std::unique_ptr<object_io_task> write_io_task)
{
    storage_engine_interface& partition_storage_engine =
        data_partition_provider_.get_partition_by_collocation(write_io_task->collocation_index_).get_storage_engine();

    //
    // At this point of execution, this scope guarantees that the
    // object container will be held for as long as the storage API takes,
    // so it is safe to only pass down the storage engine reference given
    // the storage engine reference will not be dropped by the storage engine.
    //
    status::status_code status = status::success;
    switch (write_io_task->object_request_.get_optype())
    {
        case schemas::object_request_optype::insert:
        {
            status = execute_insert_operation(
                partition_storage_engine,
                write_io_task->container_->get_engine_reference(write_io_task->collocation_index_),
                write_io_task->object_request_);
            break;
        }
        case schemas::object_request_optype::remove:
        {
            status = execute_remove_operation(
                partition_storage_engine,
                write_io_task->container_->get_engine_reference(write_io_task->collocation_index_),
                write_io_task->object_request_);
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
                static_cast<std::uint8_t>(write_io_task->object_request_.get_optype()),
                write_io_task->object_request_.get_object_id(),
                write_io_task->object_request_.get_container_name());

            status = status::invalid_operation;
            break;
        }
    }

    //
    // If the operation was successful, insert the object into the frontline cache.
    // Doing this before returning a response back to the client
    // guarantees that all future request see a consistent state for the object.
    // This provides strong consistency for get operations after a well-acknowledged object insertion.
    //
    if (status::succeeded(status))
    {
        cache_accessor_.insert_object_into_cache(
            write_io_task->object_request_);
    }

    //
    // Provide the response back to the client over the async callback.
    //
    network::server::send_response(
        write_io_task->response_callback_,
        status);
}

status::status_code
write_io_dispatcher::execute_insert_operation(
    storage_engine_interface& partition_storage_engine,
    storage_engine_reference_handle* engine_reference,
    const schemas::object_request& object_request)
{
    status::status_code status = partition_storage_engine.insert_object(
        engine_reference,
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
    storage_engine_interface& partition_storage_engine,
    storage_engine_reference_handle* engine_reference,
    const schemas::object_request& object_request)
{
    status::status_code status = partition_storage_engine.remove_object(
        engine_reference,
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

} // namespace lazarus::storage.