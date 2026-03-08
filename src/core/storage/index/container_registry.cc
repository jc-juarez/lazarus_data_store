// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_registry.cc'
// Author: jcjuarez
// Description:
//      Registry for storing container names and their
//      respective storage engine references.
// ****************************************************

#include <unordered_set>
#include <spdlog/spdlog.h>
#include "container_registry.hh"
#include "../../startup/collocation_builder.hh"

namespace lazarus
{
namespace storage
{

container_registry::container_registry()
{}

void
container_registry::register_container_reference(
    const std::string& container_name,
    storage_engine_reference_handle* engine_reference)
{
    references_map_[container_name].emplace_back(engine_reference);
}

status::status_code
container_registry::execute_integrity_validation(
    const std::string& container_name) const
{
    if (references_map_.find(container_name) == references_map_.end())
    {
        spdlog::critical("Unknown container name to the container registry. "
            "ContainerName={}.",
            container_name);

        return status::container_not_exists;
    }

    //
    // Ensure the container has all data partition instances
    // and make sure all references are different.
    //
    auto& container_references = references_map_.at(container_name);
    if (container_references.size() != collocation_builder::k_number_collocations)
    {
        spdlog::critical("Container contains unexpected number of storage engine references. "
            "ContainerName={}, "
            "ExpectedNumEngineReferences={}, "
            "FoundNumEngineReferences={}.",
            container_name,
            collocation_builder::k_number_collocations,
            container_references.size());

        return status::invalid_number_engine_references;
    }

    std::unordered_set<storage_engine_reference_handle*> engine_references;
    for (auto& engine_reference : container_references)
    {
        engine_references.insert(engine_reference);
    }

    if (engine_references.size() != collocation_builder::k_number_collocations)
    {
        spdlog::critical("Container contains duplicate storage engine references. "
            "ContainerName={}, "
            "ExpectedNumEngineReferences={}, "
            "FoundNumEngineReferences={}.",
            container_name,
            collocation_builder::k_number_collocations,
            container_references.size());

        return status::duplicate_engine_references;
    }

    return status::success;
}

std::optional<std::vector<storage_engine_reference_handle*>>
container_registry::get_references(
    const std::string& container_name) const
{
    if (references_map_.find(container_name) != references_map_.end())
    {
        return std::make_optional<std::vector<storage_engine_reference_handle*>>(
            references_map_.at(container_name));
    }

    return std::nullopt;
}

} // namespace storage.
} // namespace lazarus.