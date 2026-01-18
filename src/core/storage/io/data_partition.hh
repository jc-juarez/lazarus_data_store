// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'data_partition.hh'
// Author: jcjuarez
// Description:
//      Data storage instance for IO access.
// ****************************************************

#pragma once

#include <memory>
#include <cstdint>

namespace lazarus
{
namespace storage
{

class storage_engine;

class data_partition
{
public:

    //
    // Constructor.
    // A valid storage engine instance must be provided.
    //
    data_partition(
        const std::uint16_t collocation_index,
        std::unique_ptr<storage_engine> storage_engine);

    //
    // Provides access to the underlying storage engine.
    //
    storage_engine&
    get_storage_engine();

private:

    //
    // Storage engine for the data partition.
    // Access to the engine should be done while always
    // owning a valid reference to the data partition.
    //
    std::unique_ptr<storage_engine> storage_engine_;

    //
    // Drive collocation index.
    //
    std::uint16_t collocation_index_;
};

} // namespace storage.
} // namespace lazarus.