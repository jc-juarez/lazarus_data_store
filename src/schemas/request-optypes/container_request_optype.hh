// ****************************************************
// Lazarus Data Store
// Schemas
// 'container_request_optype.hh'
// Author: jcjuarez
// Description:
//      Optypes for object container requests. 
// ****************************************************

#pragma once

#include <cstdint>

namespace lazarus
{
namespace schemas
{

//
// Optype for describing object container operations.
//
enum class container_request_optype : std::uint8_t
{
    //
    // Invalid optype. Operation should not be processed.
    //
    invalid = 0,


    // Request optype for creating a new object container.
    //
    create = 1,

    //
    // Request optype for deleting an existing object container.
    //
    remove = 2
};

} // namespace schemas.
} // namespace lazarus.