// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'cache_accessor.cc'
// Author: jcjuarez
// Description:
//      Provides access to object update operations
//      against the frontline cache.
// ****************************************************

#include <spdlog/spdlog.h>
#include "cache_accessor.hh"
#include "frontline_cache.hh"

namespace lazarus
{
namespace storage
{

cache_accessor::cache_accessor(
    std::shared_ptr<frontline_cache> frontline_cache)
    : frontline_cache_{std::move(frontline_cache)}
{}

void
cache_accessor::insert_object_into_cache(
    schemas::object_request& object_request)
{
    //
    // Create a copy of these parameters since
    // they will be moved after the cache insertion.
    //
    const std::string object_id = object_request.get_object_id();
    const std::string container_name = object_request.get_container_name();
    const status::status_code status = frontline_cache_->put(
        std::move(object_request.get_object_id_mutable()),
        std::move(object_request.get_object_data_mutable()),
        std::move(object_request.get_container_name_mutable()));

    if (status::succeeded(status))
    {
        spdlog::info("Frontline cache object insertion succeeded on insert object operation. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_id,
            container_name);
    }
    else
    {
        spdlog::error("Frontline cache object insertion failed on insert object operation. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_id,
            container_name,
            status);
    }
}

void
cache_accessor::remove_object_from_cache(
    schemas::object_request& object_request)
{}

} // namespace storage.
} // namespace lazarus.