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
#include "../storage/io/data_partition_table.hh"
#include "../storage/io/threading_context_table.hh"
#include "../storage/io/data_partition_provider.hh"
#include "../storage/io/threading_context_provider.hh"
#include "../storage/io/collocation_resolver.hh" // remove.

namespace lazarus
{
namespace storage
{

collocation_builder::collocation_builder() = default;

std::tuple<std::unique_ptr<data_partition_provider>, std::unique_ptr<threading_context_provider>>
collocation_builder::generate_collocation_topology(
    const storage_configuration& storage_configuration)
{
    // Move this outside.
    auto collocation_resolver = std::make_shared<storage::collocation_resolver>(k_number_collocations);

    //
    // The system should create the same number of topology subtypes.
    // This is crucial as to maintain IO requirements for the storage engine.
    //
    auto dp_table = std::make_unique<data_partition_table>();
    auto tc_table = std::make_unique<threading_context_table>();
    auto dp_provider = std::make_unique<data_partition_provider>(std::move(dp_table), collocation_resolver);
    auto tc_provider = std::make_unique<threading_context_provider>();

    for (std::uint16_t collocation_index = 0u; collocation_index < k_number_collocations; ++collocation_index)
    {

    }

    return std::make_tuple(std::move(dp_provider), std::move(tc_provider));
}

} // namespace storage.
} // namespace lazarus.