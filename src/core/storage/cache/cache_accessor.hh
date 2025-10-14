// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'cache_accessor.hh'
// Author: jcjuarez
// Description:
//      Provides access to object update operations
//      against the frontline cache.
// ****************************************************

#pragma once

#include "../../status/status.hh"
#include "../../common/aliases.hh"
#include "../../schemas/request-interfaces/object_request.hh"

namespace lazarus
{
namespace storage
{

class frontline_cache;

class cache_accessor
{
public:

    //
    // Constructor.
    //
    cache_accessor(
        std::shared_ptr<frontline_cache> frontline_cache);

    //
    // Executes an insertion operation into the frontline cache.
    //
    void
    insert_object_into_cache(
        schemas::object_request& object_request);

    //
    // Executes a deletion operation from the frontline cache.
    //
    void
    remove_object_from_cache(
        schemas::object_request& object_request);

private:

    //
    // Frontline cache handle.
    //
    std::shared_ptr<frontline_cache> frontline_cache_;
};

} // namespace storage.
} // namespace lazarus.