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