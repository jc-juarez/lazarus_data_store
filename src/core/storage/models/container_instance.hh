// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_instance.hh'
// Author: jcjuarez
// Description:
//      Data partition related metadata for
//      containers.
// ****************************************************

#pragma once

#include "../../common/aliases.hh"
#include "../io/storage_engine_interface.hh"

namespace lazarus
{
namespace storage
{

struct container_instance
{
    container_instance(
        const std::uint16_t collocation_index,
        storage_engine_interface& storage_engine,
        storage_engine_reference* storage_engine_reference)
        : collocation_index_{collocation_index},
          storage_engine_{storage_engine},
          storage_engine_reference_{storage_engine_reference}
    {}

    //
    // Associated collocation index.
    //
    const std::uint16_t collocation_index_;

    //
    // Storage engine reference handle.
    //
    storage_engine_interface& storage_engine_;

    //
    // Pointer to the associated column family for the object container.
    //
    storage_engine_reference* const storage_engine_reference_;
};

} // namespace storage.
} // namespace lazarus.