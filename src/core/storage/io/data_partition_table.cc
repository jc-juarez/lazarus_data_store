// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'data_partition_table.cc'
// Author: jcjuarez
// Description:
//      Contains all data partitions in the system.
// ****************************************************

#include "storage_engine.hh"
#include "data_partition.hh"
#include "data_partition_table.hh"

namespace lazarus
{
namespace storage
{

data_partition_table::data_partition_table()
{}

void
data_partition_table::append_partition(
    const std::string& partition_prefix,
    const std::uint16_t collocation_index,
    const storage_configuration& storage_configuration,
    std::unique_ptr<storage_engine> storage_engine)
{
    partitions_.emplace_back(
        std::make_shared<data_partition>(
            partition_prefix,
            collocation_index,
            storage_configuration,
            std::move(storage_engine)));
}

std::shared_ptr<data_partition>
data_partition_table::get_partition(
    const std::uint16_t collocation_index)
{
    return partitions_.at(collocation_index);
}

std::vector<std::shared_ptr<data_partition>>
data_partition_table::get_all_partitions()
{
    return partitions_;
}

} // namespace storage.
} // namespace lazarus.