// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'threading_context.cc'
// Author: jcjuarez
// Description:
//      Scheduling context for executing object IO
//      operations.
// ****************************************************

#include "threading_context.hh"

namespace pandora
{
namespace storage
{

threading_context::threading_context(
    const std::uint16_t collocation_index)
    : collocation_index_{collocation_index}
{}

} // namespace storage.
} // namespace pandora.