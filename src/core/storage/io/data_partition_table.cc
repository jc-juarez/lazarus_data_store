// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'data_partition_table.cc'
// Author: jcjuarez
// Description:
//      Contains all data partitions in the system.
// ****************************************************

#include "storage_engine.hh"
#include "data_partition_table.hh"

namespace pandora
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
        partition_prefix,
        collocation_index,
        storage_configuration,
        std::move(storage_engine));
}

data_partition&
data_partition_table::get_partition(
    const std::uint16_t collocation_index)
{
    return partitions_.at(collocation_index);
}

std::span<data_partition>
data_partition_table::get_all_partitions()
{
    return partitions_;
}

//
// Returns the total number of data partitions.
//
std::uint16_t
data_partition_table::get_num_data_partitions()
{
    return partitions_.size();
}

} // namespace storage.
} // namespace pandora.