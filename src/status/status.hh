// ****************************************************
// Lazarus Data Store
// Status
// 'status.hh'
// Author: jcjuarez
// Description:
//      Status codes for error handling. 
// ****************************************************

#pragma once

#include <cstdint>

namespace lazarus
{
namespace status
{

//
// Status code type alias.
//
using status_code = std::uint32_t;

//
// Status code definition macro.
//
#define status_code_definition(status_code_name, status_code_value) static constexpr status_code status_code_name = status_code_value

//
// Operation succeeded.
//
status_code_definition(success, 0x0'0000000);

//
// Generic operation failed.
//
status_code_definition(fail, 0x8'0000000);

//
// Determines whether a given status is considered as failure.
//
inline static
bool
failed(
    const status_code status_code)
{
    return static_cast<int>(status_code) < 0;
}

//
// Determines whether a given status is considered as success.
//
inline static
bool
succeeded(
    const status_code& status_code)
{
    return status_code == success;
}

//
// Determines whether a given status is the same as another status.
//
inline static
bool
is_same(
    const status_code& status_code_left,
    const status_code& status_code_right)
{
    return status_code_left == status_code_right;
}

//
// Determines whether a given status is not the same as another status.
//
inline static
bool
is_not_same(
    const status_code& status_code_left,
    const status_code& status_code_right)
{
    return status_code_left != status_code_right;
}

//
// *****************************************
// Status codes for the lazarus data store.
// *****************************************
//

//
// Object container already exists.
//
status_code_definition(object_container_already_exists, 0x8'0000001);

//
// Invalid state for a given operation.
//
status_code_definition(invalid_operation, 0x8'0000002);

} // namespace status.
} // namespace lazarus.