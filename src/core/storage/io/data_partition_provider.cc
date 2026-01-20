// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
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
#include "collocation_resolver.hh"
#include "data_partition_provider.hh"

namespace lazarus
{
namespace storage
{

data_partition_provider::data_partition_provider(
    std::unique_ptr<data_partition_table> data_partition_table,
    std::shared_ptr<collocation_resolver> collocation_resolver)
    : partitions_table_{std::move(data_partition_table)},
      collocation_resolver_{std::move(collocation_resolver)}
{}

std::shared_ptr<data_partition>
data_partition_provider::get_partition_by_key(
    const std::string& object_key)
{
    //
    // First, obtain the corresponding collocation for the given
    // object, then use such collocation for the lookup.
    //
    const std::uint16_t collocation_index = collocation_resolver_->get_collocation_index_for_key(
        object_key);

    return get_partition_by_collocation(collocation_index);
}

std::shared_ptr<data_partition>
data_partition_provider::get_partition_by_collocation(
    const std::uint16_t collocation_index)
{
    return partitions_table_->get_partition(collocation_index);
}

} // namespace storage.
} // namespace lazarus.