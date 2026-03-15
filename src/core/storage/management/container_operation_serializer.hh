// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_operation_serializer.hh'
// Author: jcjuarez
// Description:
//      Manages object container operations
//      in a serialized manner. 
// ****************************************************

#pragma once

#include <memory>
#include <expected>
#include "../../status/status.hh"
#include "../../network/server/server.hh"
#include "../../common/task_serializer.hh"
#include "../models/container_instance.hh"
#include "../../schemas/request-interfaces/container_request.hh"

namespace lazarus
{
namespace storage
{

class container_index;
class data_partition_provider;
class storage_engine_interface;

//
// Serialized object container operation orchestrator.
//
class container_operation_serializer
{
public:

    //
    // Constructor for the object operation serializer.
    //
    container_operation_serializer(
        data_partition& metadata_partition,
        data_partition_provider& data_partition_provider,
        container_index& container_index);

    //
    // Enqueues a serialized object container operation into the serializer.
    // Handles server response callback replies in an asynchronous manner.
    //
    void
    enqueue_container_operation(
        schemas::container_request&& container_request,
        network::server_response_callback&& response_callback);

private:

    //
    // Object container operation serialization entry point.
    // All calls to this proxy are serialized.
    //
    void
    container_operation_serial_proxy(
        const schemas::container_request& container_request,
        const network::server_response_callback& response_callback);

    //
    // Orchestrates the object container creation process.
    //
    status::status_code
    handle_container_creation(
        const schemas::container_request& container_request);

    //
    // Orchestrates the object container removal process.
    //
    status::status_code
    handle_container_removal(
        const schemas::container_request& container_request);

    //
    // Handles the creation of the container instances
    // across all data partitions.
    // Upon success, the list of container instances is returned.
    // The resulting vector is guaranteed to contain all partition metadata
    // in order with respective to its collocation index:
    // --------------------------------------------------------------
    // | Offset_0 | Offset_1 | Offset_2 | Offset_3 | ... | Offset_N |
    // --------------------------------------------------------------
    // |  Meta_0  |  Meta_1  |  Meta_2  |  Meta_3  | ... |  Meta_N  |
    // --------------------------------------------------------------
    //
    std::expected<
        std::vector<container_instance>,
        status::status_code>
    create_container_instances_on_data_partitions(
        const std::string& container_name);

    //
    // Marks the provided engine references as
    // approved across the corresponding data partitions.
    //
    void
    mark_engine_references_as_approved(
        const std::vector<container_instance>& container_instances);

    //
    // Serializer task queue for executing object container operations serially.
    //
    common::task_serializer container_operations_serializer_;

    //
    // Reference for the data partition provider.
    //
    data_partition_provider& data_partition_provider_;

    //
    // Reference for the object container index component.
    //
    container_index& container_index_;

    //
    // Reference container metadata partition component.
    data_partition& metadata_partition_;
};

} // namespace storage.
} // namespace lazarus.