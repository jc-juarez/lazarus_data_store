// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'data_partition_table.hh'
// Author: jcjuarez
// Description:
//      Contains all data partitions in the system.
// ****************************************************

#pragma once

#include <vector>
#include <memory>
#include "data_partition.hh"

namespace lazarus
{
namespace storage
{

class storage_engine;

class data_partition_table
{
public:

    //
    // Constructor.
    //
    data_partition_table();

    //
    // Appends a new data partition instance to the table.
    //
    void
    append_partition(
        const std::string& partition_prefix,
        const std::uint16_t collocation_index,
        const storage_configuration& storage_configuration,
        std::unique_ptr<storage_engine> storage_engine);

    //
    // Returns a non-owning reference to a data partition.
    // This API is meant to be only be consumed by the data partition provider.
    //
    data_partition&
    get_partition(
        const std::uint16_t collocation_index);

    //
    // Returns a list with all data partitions in the system.
    //
    std::span<data_partition>
    get_all_partitions();

private:

    //
    // Table for holding all data partitions in an owning model.
    // References to be provided should be non-owning.
    // The table functions as a direct mapping given a collocation index:
    // --------------------------------------------------------------
    // | Offset_0 | Offset_1 | Offset_2 | Offset_3 | ... | Offset_N |
    // --------------------------------------------------------------
    // |   DP_0   |   DP_1   |   DP_2   |   DP_3   | ... |   DP_N   |
    // --------------------------------------------------------------
    //
    std::vector<data_partition> partitions_;
};

} // namespace storage.
} // namespace lazarus.