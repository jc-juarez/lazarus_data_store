// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'data_partition_provider.hh'
// Author: jcjuarez
// Description:
//      Provides access to specific data partitions.
// ****************************************************

#pragma once

#include <memory>
#include "data_partition_table.hh"

namespace lazarus
{
namespace storage
{

class data_partition;
class collocation_resolver;

class data_partition_provider
{
public:

    //
    // Constructor.
    //
    data_partition_provider(
        std::unique_ptr<data_partition_table> data_partition_table);

    //
    // Provides access to a specific data partition by collocation index lookup.
    //
    data_partition&
    get_partition_by_collocation(
        const std::uint16_t collocation_index);

    //
    // Returns a list with all data partitions in the system.
    //
    std::span<data_partition>
    get_all_partitions();

private:

    //
    // Table containing all data partitions in the system.
    //
    std::unique_ptr<data_partition_table> partitions_table_;
};

} // namespace storage.
} // namespace lazarus.