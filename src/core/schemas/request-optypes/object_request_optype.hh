// ****************************************************
// Lazarus Data Store
// Schemas
// 'object_request_optype.hh'
// Author: jcjuarez
// Description:
//      Optypes for object requests.
// ****************************************************

#pragma once

#include <cstdint>

namespace lazarus::schemas
{

//
// Optype for describing object operations.
//
enum class object_request_optype : std::uint8_t
{
    //
    // Invalid optype. Operation should not be processed.
    //
    invalid = 0,

    //
    // Request optype for inserting an object.
    //
    insert = 1,

    //
    // Request optype for getting an object.
    //
    get = 2,

    //
    // Request optype for removing an object.
    //
    remove = 3
};

} // namespace lazarus::schemas.