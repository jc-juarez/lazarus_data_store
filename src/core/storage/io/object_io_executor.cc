// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'object_io_executor.cc'
// Author: jcjuarez
// Description:
//      Executes an object IO task against the engine.
// ****************************************************

#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "object_io_executor.hh"

namespace lazarus
{
namespace storage
{

object_io_executor::object_io_executor(
    std::shared_ptr<storage_engine_interface> storage_engine)
    : storage_engine_{std::move(storage_engine)}
{}

status::status_code
object_io_executor::execute_get_operation(
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

status::status_code
object_io_executor::execute_write_batch_operation(
    storage_engine_write_batch& write_batch)
{
    status::status_code status = storage_engine_->execute_objects_write_batch(
        write_batch);

    if (status::succeeded(status))
    {
        spdlog::info("Write batch operation succeeded.");
    }
    else
    {
        spdlog::info("Write batch operation failed.");
    }

    return status;
}

} // namespace storage.
} // namespace lazarus.