// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'collocation_builder.cc'
// Author: jcjuarez
// Description:
//      Arranges and composes all data collocations.
// ****************************************************

#include "collocation_builder.hh"
#include "../storage/io/storage_engine.hh"
#include "../storage/io/data_partition.hh"
#include "../storage/io/collocation_resolver.hh"
#include "../storage/io/data_partition_table.hh"
#include "../storage/io/threading_context_table.hh"
#include "../storage/io/data_partition_provider.hh"
#include "../storage/io/threading_context_provider.hh"

namespace lazarus
{
namespace storage
{

collocation_builder::collocation_builder() = default;

std::tuple<
    std::shared_ptr<data_partition>,
    std::shared_ptr<collocation_resolver>,
    std::shared_ptr<data_partition_provider>,
    std::shared_ptr<threading_context_provider>>
collocation_builder::generate_collocation_topology(
    const storage_configuration& storage_configuration)
{
    //
    // The system should create the same number of topology subtypes.
    // This is crucial as to maintain IO requirements for the storage engine.
    //
    auto container_metadata_partition = std::make_shared<storage::data_partition>(
        k_container_metadata_partition_prefix,
        k_number_collocations, /* The index for the containers metadata corresponds to the Kth collocation. */
        storage_configuration,
        std::make_unique<storage::storage_engine>());

    //
    // Collocation resolver.
    // This routes all object keys to their respective data partitions or threading contexts.
    //
    auto collocation_resolver = std::make_shared<storage::collocation_resolver>(
        k_number_collocations);

    //
    // Tables for data partitions and threading contexts.
    //
    auto data_partitions_table = std::make_unique<data_partition_table>();
    auto threading_contexts_table = std::make_unique<threading_context_table>();

    //
    // Populate data partitions and threading contexts tables.
    // The append actions for the tables are done in exact correlation
    // relative to the respective correlation to the collocation indices:
    // --------------------------------------------------------------
    // | Offset_0 | Offset_1 | Offset_2 | Offset_3 | ... | Offset_N |
    // --------------------------------------------------------------
    // |  DP/TC_0 |  DP/TC_1 |  DP/TC_2  | DP/TC_3 | ... |  DP/TC_N |
    // --------------------------------------------------------------
    //
    for (std::uint16_t collocation_index = 0u; collocation_index < k_number_collocations; ++collocation_index)
    {
        data_partitions_table->append_partition(
            k_structured_partition_prefix,
            collocation_index,
            storage_configuration,
            std::make_unique<storage::storage_engine>());

        threading_contexts_table->append_context(
            collocation_index);
    }

    //
    // Providers for data partitions and threading contexts.
    //
    auto data_partitions_provider = std::make_shared<data_partition_provider>(
        std::move(data_partitions_table));
    auto threading_contexts_provider = std::make_shared<threading_context_provider>(
        std::move(threading_contexts_table));

    return std::make_tuple(
        container_metadata_partition,
        collocation_resolver,
        data_partitions_provider,
        threading_contexts_provider);
}

} // namespace storage.
} // namespace lazarus.