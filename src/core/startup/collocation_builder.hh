// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
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

namespace lazarus
{
namespace storage
{

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
    //
    static
    std::tuple<std::unique_ptr<data_partition_provider>, std::unique_ptr<threading_context_provider>>
    generate_collocation_topology(
        const storage_configuration& storage_configuration);

private:

    //
    // Core invariant of the system.
    // This should always remain the same.
    // A change in this value upon system restart can cause serious
    // data corruption and the inability to locate keys on data partitions.
    //
    static constexpr std::uint16_t k_number_collocations = 8u;
};

} // namespace storage.
} // namespace lazarus.