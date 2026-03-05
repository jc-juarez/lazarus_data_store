// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_reference_registry.cc'
// Author: jcjuarez
// Description:
//      Registry for storing container names and their
//      respective storage engine references.
// ****************************************************

#include <unordered_set>
#include <spdlog/spdlog.h>
#include "container_reference_registry.hh"
#include "../../startup/collocation_builder.hh"

namespace lazarus
{
namespace storage
{

container_reference_registry::container_reference_registry()
{}

void
container_reference_registry::register_container_reference(
    const std::string& container_name,
    storage_engine_reference_handle* engine_reference)
{
    references_map_[container_name].emplace_back(engine_reference);
}

status::status_code
container_reference_registry::execute_integrity_validation() const
{
    //
    // Ensure every container has all data partition instances
    // and make sure all references are different.
    //
    for (auto& container_entry : references_map_)
    {
        if (container_entry.second.size() != collocation_builder::k_number_collocations)
        {
            spdlog::critical("Container contains unexpected number of storage engine references. "
                "ContainerName={}, "
                "ExpectedNumEngineReferences={}, "
                "FoundNumEngineReferences={}.",
                container_entry.first,
                collocation_builder::k_number_collocations,
                container_entry.second.size());

            return status::invalid_number_engine_references;
        }

        std::unordered_set<storage_engine_reference_handle*> engine_references;
        for (auto& engine_reference : container_entry.second)
        {
            engine_references.insert(engine_reference);
        }

        if (engine_references.size() != collocation_builder::k_number_collocations)
        {
            spdlog::critical("Container contains duplicate storage engine references. "
                "ContainerName={}, "
                "ExpectedNumEngineReferences={}, "
                "FoundNumEngineReferences={}.",
                container_entry.first,
                collocation_builder::k_number_collocations,
                container_entry.second.size());

            return status::duplicate_engine_references;
        }
    }

    return status::success;
}

std::optional<std::vector<storage_engine_reference_handle*>>
container_reference_registry::get_references(
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