// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'read_io_executor.cc'
// Author: jcjuarez
// Description:
//      Executes an object IO task against the engine.
// ****************************************************

#include "read_io_executor.hh"
#include "data_partition_provider.hh"

namespace lazarus
{
namespace storage
{

read_io_executor::read_io_executor(
    data_partition_provider& data_partition_provider)
    : data_partition_provider_{data_partition_provider}
{}

status::status_code
read_io_executor::execute_get_operation(
    const std::uint16_t collocation_index,
    storage_engine_reference* engine_reference,
    const schemas::object_request& object_request,
    byte_stream& object_data)
{
    storage_engine_interface& partition_storage_engine =
        data_partition_provider_.get_partition_by_collocation(collocation_index).get_storage_engine();

    status::status_code status = partition_storage_engine.get_object(
        engine_reference,
        object_request.get_object_id().c_str(),
        &object_data);

    if (status::succeeded(status))
    {
        TRACE_LOG(info, "Object retrieval succeeded. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_container_name());
    }
    else
    {
        TRACE_LOG(error, "Object retrieval failed. "
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

} // namespace storage.
} // namespace lazarus.