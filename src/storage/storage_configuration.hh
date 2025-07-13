// ****************************************************
// Lazarus Data Store
// Storage
// 'storage_configuration.hh'
// Author: jcjuarez
// Description:
//      Container that stores the default
//      configurations for the storage subsystem.
// ****************************************************

#pragma once

#include <string>
#include <thread>
#include <cstdlib>
#include <stdexcept>
#include <filesystem>

namespace lazarus
{
namespace storage
{

//
// Storage configurations container.
//
struct storage_configuration
{
    //
    // Constructor for the storage configurations.
    // Specifies the default values to be used by the storage components.
    //
    storage_configuration()
        : core_database_path_{},
          garbage_collector_periodic_interval_ms_{10'000u},
          max_container_name_size_bytes_{512u},
          max_object_id_size_bytes_{1'024u},
          max_object_data_size_bytes_{1'024u * 1'024u},
          max_number_containers_{10'000},
          number_write_io_threads_{4u},
          number_read_io_threads_{std::thread::hardware_concurrency()},
          storage_engine_block_cache_size_mib_{512u},
          container_index_number_buckets_{8u}
    {
        //
        // Set the core database path with the default home directory path if no path
        // override was specified. Throws if fails to obtain the home environment variable.
        //
        const char* home_environment_variable = std::getenv("HOME");

        if (home_environment_variable == nullptr)
        {
            throw std::runtime_error("Failed to access the HOME environment "
                "variable for setting the core database path.");
        }

        core_database_path_ =
            std::string(home_environment_variable) + "/lazarus/lazarus-ds";

        //
        // Create the path if it does not exist.
        //
        std::filesystem::create_directories(core_database_path_);
    }

    //
    // Directory path for the core database.
    //
    std::string core_database_path_;

    //
    // Periodic garbage collector interval in milliseconds.
    //
    std::uint32_t garbage_collector_periodic_interval_ms_;

    //
    // Max size for an object container in bytes.
    //
    std::uint32_t max_container_name_size_bytes_;

    //
    // Max size for an object ID in bytes.
    //
    std::uint32_t max_object_id_size_bytes_;

    //
    // Max size for the object data content in bytes.
    //
    std::uint32_t max_object_data_size_bytes_;

    //
    // Max number of object containers to be present on the data store.
    //
    std::uint32_t max_number_containers_;

    //
    // Number of threads for the write IO dispatcher thread pool.
    // Should ideally be a low number as to avoid oversubscribing the storage engine.
    //
    std::uint32_t number_write_io_threads_;

    //
    // Number of threads for the read IO dispatcher thread pool.
    // The engine allows efficient retrieval with high concurrency,
    // so the total number of logical cores is ideal.
    //
    std::uint32_t number_read_io_threads_;

    //
    // Storage engine block cache size in MiB.
    //
    std::uint64_t storage_engine_block_cache_size_mib_;

    //
    // Number of buckets for the container index.
    //
    std::uint16_t container_index_number_buckets_;
};

} // namespace storage.
} // namespace lazarus.