// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
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