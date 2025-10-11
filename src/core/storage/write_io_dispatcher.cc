// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
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

#include "container.hh"
#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "frontline_cache.hh"
#include "write_io_dispatcher.hh"

namespace lazarus::storage
{

write_io_dispatcher::write_io_dispatcher(
    std::shared_ptr<storage_engine_interface> storage_engine,
    std::shared_ptr<frontline_cache> frontline_cache)
    : storage_engine_{std::move(storage_engine)},
      frontline_cache_{std::move(frontline_cache)}
{}

void
write_io_dispatcher::start()
{}

void
write_io_dispatcher::enqueue_io_task(
    schemas::object_request&& object_request,
    std::shared_ptr<container> container,
    network::server_response_callback&& response_callback)
{}

void
write_io_dispatcher::wait_for_stop()
{}

void
write_io_dispatcher::dispatch_write_io_operations(
    schemas::object_request&& object_request,
    std::shared_ptr<container> container,
    network::server_response_callback&& response_callback)
{}

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

void write_io_dispatcher::insert_object_into_cache(
    schemas::object_request& object_request)
{
    //
    // Insert the object into the cache if the operation was an insertion.
    // Removals are not tracked as part of the caching strategy.
    //
    if (object_request.get_optype() != schemas::object_request_optype::insert)
    {
        return;
    }

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
        spdlog::info("Frontline cache object insertion succeeded on insert object operation. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_id,
            container_name);
    }
    else
    {
        spdlog::error("Frontline cache object insertion failed on insert object operation. "
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

} // namespace lazarus::common.