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

status::status_code
container_registry::register_container_reference(
    const std::string& container_name,
    const std::uint16_t collocation_index,
    storage_engine_reference_handle* engine_reference)
{
    if (references_discovered_.find(engine_reference) != references_discovered_.end())
    {
        //
        // This engine reference had already been discovered, this is
        // an inconsistent state given every reference for every container across each
        // data partition should be different.
        //
        TRACE_LOG(critical, "Found duplicate engine reference in the container registry. "
            "ContainerName={}, "
            "DuplicateEngineReference={}, "
            "CurrentCollocationIndex={}, "
            "PreviouslyRegisteredCollocationIndex={}.",
            container_name,
            static_cast<void*>(engine_reference),
            collocation_index,
            references_discovered_.at(engine_reference));

        return status::duplicate_engine_references;
    }

    references_discovered_.emplace(
        engine_reference,
        collocation_index);

    //
    // If not already present, create a list of Kth
    // collocation entries filled with an invalid reference.
    //
    references_map_.try_emplace(
        container_name,
        collocation_builder::k_number_collocations,
        nullptr);
    references_map_.at(container_name).at(collocation_index) = engine_reference;

    return status::success;
}

status::status_code
container_registry::execute_integrity_validation(
    const std::string& container_name) const
{
    if (references_map_.find(container_name) == references_map_.end())
    {
        TRACE_LOG(critical, "Unknown container name to the container registry. "
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
        TRACE_LOG(critical, "Container contains unexpected number of storage engine references. "
            "ContainerName={}, "
            "ExpectedNumEngineReferences={}, "
            "FoundNumEngineReferences={}.",
            container_name,
            collocation_builder::k_number_collocations,
            container_references.size());

        return status::invalid_number_engine_references;
    }

    //
    // Iterate over all engine reference slots and make sure
    // the container was present on every data partition. This is essentially
    // a filesystem scan for ensuring the container is present and not corrupted.
    //
    for (auto& reference : container_references)
    {
        if (reference == nullptr)
        {
            TRACE_LOG(critical, "Container is not present on all filesystem structured data partitions. "
                "ContainerName={}, "
                "ContainerFilesystemLayout={}.",
                container_name,
                format_references_list(container_references));

            return status::invalid_number_engine_references;
        }
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

std::string
container_registry::format_references_list(
    const std::vector<storage_engine_reference_handle*>& references)
{
    std::stringstream ss;
    ss << "{";

    for (std::uint16_t collocation_index = 0u; collocation_index < references.size(); ++collocation_index)
    {
        ss
        << "{CollocationIndex="
        << collocation_index << "=";

        auto* reference = references.at(collocation_index);

        if (reference == nullptr)
        {
            ss << "NotFound";
        }
        else
        {
            ss
            << "0x" << std::hex
            << reinterpret_cast<std::uintptr_t>(reference)
            << std::dec;
        }

        ss << "}";
    }

    ss << "}";
    return ss.str();
}

} // namespace storage.
} // namespace lazarus.