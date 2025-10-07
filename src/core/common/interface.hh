// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Common
// 'interface.hh'
// Author: jcjuarez
// Description:
//      Base interface struct for enforcing virtual
//      destruction guarantees for polymorphism.
// ****************************************************

#pragma once

namespace lazarus
{
namespace common
{

struct interface
{
    virtual ~interface() = default;
};

} // namespace common.
} // namespace lazarus.