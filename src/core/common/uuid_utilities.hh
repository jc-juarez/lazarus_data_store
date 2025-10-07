// ****************************************************
// Lazarus Data Store
// Common
// 'uuid_utilities.hh'
// Author: jcjuarez
// Description:
//      Provides UUID-related utilities. 
// ****************************************************

#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace lazarus
{
namespace common
{

//
// Generates a new UUID.
// Thread-safe function.
//
boost::uuids::uuid
generate_uuid();

inline
std::string
uuid_to_string(
    const boost::uuids::uuid p_uuid)
{
    return boost::uuids::to_string(p_uuid);
}

} // namespace common.
} // namespace lazarus.