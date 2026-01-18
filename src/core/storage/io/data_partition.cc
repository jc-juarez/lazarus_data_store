// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'data_partition.cc'
// Author: jcjuarez
// Description:
//      Data storage instance for IO access.
// ****************************************************

#include "data_partition.hh"
#include "storage_engine.hh"

namespace lazarus
{
namespace storage
{

data_partition::data_partition(
    const std::uint16_t collocation_index,
    std::unique_ptr<storage_engine> storage_engine)
    : collocation_index_{collocation_index},
      storage_engine_{std::move(storage_engine)}
{}

storage_engine&
data_partition::get_storage_engine()
{
    return *(storage_engine_.get());
}

} // namespace storage.
} // namespace lazarus.