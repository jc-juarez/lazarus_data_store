// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'threading_context.cc'
// Author: jcjuarez
// Description:
//      Scheduling context for executing object IO
//      operations.
// ****************************************************

#include "threading_context.hh"

namespace lazarus
{
namespace storage
{

threading_context::threading_context(
    const std::uint16_t collocation_index)
    : collocation_index_{collocation_index}
{}

} // namespace storage.
} // namespace lazarus.