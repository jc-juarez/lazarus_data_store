// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_reference_registry.hh'
// Author: jcjuarez
// Description:
//      Registry for storing container names and their
//      respective storage engine references.
// ****************************************************

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "../../status/status.hh"
#include "../../common/aliases.hh"

namespace lazarus
{
namespace storage
{

class container_reference_registry
{
public:

    //
    // Constructor.
    //
    container_reference_registry();

    //
    // Iterators for range traversal support.
    //
    auto begin() { return references_map_.begin(); }
    auto end() { return references_map_.end(); }
    auto begin() const { return references_map_.begin(); }
    auto end() const { return references_map_.end(); }

    //
    // Registers a container reference entry to its respective references list.
    // Every new registered reference is assumed to corresponds to its respective collocation index.
    //
    void
    register_container_reference(
        const std::string& container_name,
        storage_engine_reference_handle* engine_reference);

    //
    // Validates the required integrity checks for the registered references.
    //
    status::status_code
    execute_integrity_validation() const;

    //
    // Gets the list of container references for a given container.
    //
    std::optional<std::vector<storage_engine_reference_handle*>>
    get_references(
        const std::string& container_name) const;

private:

    //
    // Internal map for holding the container names and their respective references.
    //
    std::unordered_map<std::string, std::vector<storage_engine_reference_handle*>> references_map_;
};

} // namespace storage.
} // namespace lazarus.