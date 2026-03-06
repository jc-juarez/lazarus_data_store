// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container.hh'
// Author: jcjuarez
// Description:
//      Object container metadata structure. Object
//      containers are logical buckets for holding
//      closely related data inside the data store. 
// ****************************************************

#include <format>
#include "container.hh"
#include <spdlog/spdlog.h>
#include "../io/storage_engine.hh"

namespace lazarus
{
namespace storage
{

container::container(
    const schemas::container_persistent_interface& container_persistent_metadata,
    const std::vector<container_partition_metadata>& container_instances)
    : container_persistent_metadata_{container_persistent_metadata},
      container_instances_{container_instances},
      is_deleted_{false}
{}

container::~container()
{
    spdlog::info("Removing last object container reference from memory. "
        "ObjectContainerMetadata={}.",
        to_string());

    for (auto& container_instance : container_instances_)
    {
        status::status_code status = container_instance.storage_engine_.close_container_storage_engine_reference(
            container_instance.storage_engine_reference_);

        if (status::failed(status))
        {
            spdlog::warn("Failed to close container storage engine reference. "
                "ObjectContainerMetadata={}, "
                "DataPartitionCollocationIndex={}, "
                "StorageEngineReference={}.",
                to_string(),
                container_instance.collocation_index_,
                static_cast<void*>(container_instance.storage_engine_reference_));
        }
    }
}

schemas::container_persistent_interface
container::create_container_persistent_metadata(
    const char* container_name)
{
    schemas::container_persistent_interface container_persistent_metadata;

    //
    // Default values upon creation.
    //
    container_persistent_metadata.set_name(container_name);

    return container_persistent_metadata;
}

std::string
container::get_name() const
{
    std::shared_lock<std::shared_mutex> lock {lock_};
    return container_persistent_metadata_.name();
}

void
container::mark_as_deleted()
{
    std::unique_lock<std::shared_mutex> lock {lock_};
    is_deleted_ = true;
}

bool
container::is_deleted() const
{
    std::shared_lock<std::shared_mutex> lock {lock_};
    return is_deleted_;
}

std::string
container::to_string() const
{
    std::shared_lock<std::shared_mutex> lock {lock_};

    auto generate_instances_string = [this]()
    {
        std::stringstream ss;
        ss << "{";

        for (auto& container_instance : this->container_instances_)
        {
            ss
            << "{CollocationIndex="
            << container_instance.collocation_index_
            << ",StorageEngineReference="
            << container_instance.storage_engine_reference_
            << "}";
        }

        ss << "}";
        return ss.str();
    };

    return std::format(
        "{{Name={}, "
        "InstancesMetadata={}, "
        "IsDeleted={}}}",
        container_persistent_metadata_.name(),
        generate_instances_string(),
        is_deleted_);
}

storage_engine_reference_handle*
container::get_engine_reference(
    const std::uint16_t collocation_index)
{
    std::shared_lock<std::shared_mutex> lock {lock_};

    //
    // The container instances vector is already indexed by the collocation index.
    //
    return container_instances_.at(collocation_index).storage_engine_reference_;
}

std::vector<container_partition_metadata>
container::get_container_instances()
{
    std::shared_lock<std::shared_mutex> lock {lock_};
    return container_instances_;
}

} // namespace storage.
} // namespace lazarus.