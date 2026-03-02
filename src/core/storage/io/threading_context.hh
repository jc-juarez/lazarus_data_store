// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'threading_context.hh'
// Author: jcjuarez
// Description:
//      Scheduling context for executing object IO
//      operations.
// ****************************************************

#pragma once

#include <cstdint>

namespace lazarus
{
namespace storage
{

class threading_context
{
public:

    //
    // Constructor.
    //
    threading_context(
        const std::uint16_t collocation_index);

private:

    //
    // Corresponding data collocation index.
    //
    const std::uint16_t collocation_index_;
};

} // namespace storage.
} // namespace lazarus.