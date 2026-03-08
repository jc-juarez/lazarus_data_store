// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'read_io_executor.hh'
// Author: jcjuarez
// Description:
//      Executes an object IO task against the engine.
// ****************************************************

#pragma once

#include "../../status/status.hh"
#include "../../common/aliases.hh"
#include "../../schemas/request-interfaces/object_request.hh"

namespace lazarus
{
namespace storage
{

class data_partition_provider;

class read_io_executor
{
public:

    //
    // Constructor.
    //
    read_io_executor(
        data_partition_provider& data_partition_provider);

    //
    // Executes a get operation with the storage engine.
    //
    status::status_code
    execute_get_operation(
        storage_engine_reference_handle* container_storage_engine_reference,
        const schemas::object_request& object_request,
        byte_stream& object_data);

    //
    // Executes a batch of write operations with the storage engine.
    //
    status::status_code
    execute_write_batch_operation(
        storage_engine_write_batch& write_batch);

private:

    //
    // Reference for the data partition provider.
    //
    data_partition_provider& data_partition_provider_;
};

} // namespace storage.
} // namespace lazarus.