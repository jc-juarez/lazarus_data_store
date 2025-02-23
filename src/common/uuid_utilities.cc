// ****************************************************
// Lazarus Data Store
// Common
// 'uuid_utilities.cc'
// Author: jcjuarez
// Description:
//      Provides UUID-related utilities. 
// ****************************************************

#include "uuid_utilities.hh"

namespace lazarus
{
namespace common
{

boost::uuids::uuid
generate_uuid()
{
    thread_local boost::uuids::random_generator uuid_generator;

    return uuid_generator();
}

} // namespace common.
} // namespace lazarus.