// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Common
// 'startable_interface.hh'
// Author: jcjuarez
// Description:
//      Interface for components that need to be
//      started through a calling method.
// ****************************************************

#pragma once

#include "interface.hh"

namespace lazarus
{
namespace common
{

class startable_interface : interface
{
public:

    //
    // Starts the component.
    //
    virtual
    void
    start() = 0;
};

} // namespace common.
} // namespace lazarus.