// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'collocation_builder.hh'
// Author: jcjuarez
// Description:
//      Arranges and composes all data collocations.
// ****************************************************

#pragma once

#include <tuple>
#include <memory>
#include <cstdint>
#include "../storage/storage_configuration.hh"

namespace pandora
{
namespace storage
{

class data_partition;
class cache_accessor;
class collocation_resolver;
class data_partition_provider;
class threading_context_provider;

class collocation_builder
{
public:

    //
    // Constructor.
    //
    collocation_builder();

    //
    // Generates and returns the list of data partitions
    // and threading contexts to be used in the system within a fixed-topology set.
    // Returns:
    // 1. The data partition for the internal container metadata. This is considered as the last partition.
    // 2. The collocation resolver for routing object keys to their respective partition.
    // 3. The data partition provider for accessing all partitions.
    // 4. The threading context provider for accessing all IO concurrency contexts.
    //
    static
    std::tuple<
        std::unique_ptr<data_partition>,
        std::unique_ptr<collocation_resolver>,
        std::unique_ptr<data_partition_provider>,
        std::unique_ptr<threading_context_provider>>
    generate_collocation_topology(
        const storage_configuration& storage_configuration,
        cache_accessor& cache_accessor);

    //
    // Core invariant of the system.
    // This should always remain the same.
    // A change in this value upon system restart can cause serious
    // data corruption and the inability to locate keys on data partitions.
    //
    static constexpr std::uint16_t k_number_collocations = 8u;

private:

    //
    // Partition prefix to use for structured data partitions (SDPs).
    //
    static constexpr const char* k_structured_partition_prefix = "dp";

    //
    // Partition prefix to use for the container metadata partition.
    //
    static constexpr const char* k_metadata_partition_prefix = "meta";
};

} // namespace storage.
} // namespace pandora.