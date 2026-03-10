// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'data_partition_provider.cc'
// Author: jcjuarez
// Description:
//      Provides access to specific data partitions.
// ****************************************************

#include "data_partition.hh"
#include "data_partition_table.hh"
#include "data_partition_provider.hh"

namespace lazarus
{
namespace storage
{

data_partition_provider::data_partition_provider(
    std::unique_ptr<data_partition_table> data_partition_table)
    : partitions_table_{std::move(data_partition_table)}
{}

data_partition&
data_partition_provider::get_partition_by_collocation(
    const std::uint16_t collocation_index)
{
    return partitions_table_->get_partition(collocation_index);
}

std::span<data_partition>
data_partition_provider::get_all_partitions()
{
    return partitions_table_->get_all_partitions();
}

} // namespace storage.
} // namespace lazarus.