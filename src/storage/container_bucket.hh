// ****************************************************
// Lazarus Data Store
// Storage
// 'container_bucket.hh'
// Author: jcjuarez
// Description:
//      Fundamental structure for storing containers.
// ****************************************************

#pragma once

#include <tbb/tbb.h>
#include "container.hh"
#include "../status/status.hh"

namespace lazarus::storage
{

class storage_engine;

class container_bucket
{
public:

    //
    // Constructor.
    //
    container_bucket(
        std::shared_ptr<storage_engine> storage_engine);

    //
    // Sets the container bucket index identifier.
    // Not thread-safe under the core assumption that this should never
    // be called other than in the constructor of the container index.
    //
    void
    set_index(
        const std::uint16_t index);

    //
    // Inserts a container into the bucket.
    //
    status::status_code
    insert_container(
        storage_engine_reference_handle* storage_engine_reference,
        const schemas::container_persistent_interface& container_persistent_metadata);

    //
    // Gets a reference of an object container.
    // Returns nullptr in case it does not exist.
    //
    std::shared_ptr<container>
    get_container(
        const std::string& container_name) const;

    //
    // Gets a list of all the object containers in the bucket.
    //
    std::vector<std::shared_ptr<container>>
    get_all_containers() const;

    //
    // Removes an object container from the index table. After this, all subsequent
    // API calls to the object container index will not contain the object container.
    //
    status::status_code
    remove_container(
        const std::string& container_name);

    //
    // Gets the total current number of
    // object containers in the bucket.
    // This API returns the total number of active and soft-deleted containers.
    //
    std::size_t
    get_number_containers() const;

private:

    //
    // Container bucket index.
    //
    std::uint16_t index_;

    //
    // Holds object containers in the system.
    // Maps an object container identifier to the
    // respective object container memory reference.
    //
    tbb::concurrent_hash_map<std::string, std::shared_ptr<container>> container_bucket_map_;

    //
    // Handle for the storage engine.
    //
    std::shared_ptr<storage_engine> storage_engine_;
};

} // namespace lazarus::storage.