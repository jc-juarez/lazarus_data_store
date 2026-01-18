// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'task_router.cc'
// Author: jcjuarez
// Description:
//      Routes object keys to the corresponding
//      collocation index to be used by providers.
// ****************************************************

#include "task_router.hh"

namespace lazarus
{
namespace storage
{

task_router::task_router(
    const std::uint16_t number_data_collocations)
    : number_data_collocations_{number_data_collocations}
{}

std::uint16_t
task_router::get_collocation_index_for_key(
    const std::string& object_key)
{
    return hasher_(object_key) % number_data_collocations_;
}

} // namespace storage.
} // namespace lazarus.