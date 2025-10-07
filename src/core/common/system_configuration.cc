// ****************************************************
// Lazarus Data Store
// Common
// 'system_configuration.cc'
// Author: jcjuarez
// Description:
//      Container for storing all system
//      configurations.
// ****************************************************

#include <format>
#include <fstream>
#include "system_configuration.hh"

namespace lazarus
{
namespace common
{

void
system_configuration::load_configuration_from_file(
    const std::string& config_file_path)
{
    std::ifstream config_file{config_file_path};

    if (!config_file.is_open())
    {
        throw std::runtime_error(std::format(
            "Failed to open the provided configuration file path for starting lazarus. "
            "ConfigFilePath={}.",
            config_file_path));
    }

    nlohmann::json config;
    config_file >> config;

    //
    // Load the logger section if available.
    //
    if (config.contains(logger_config_section_name_))
    {
        const auto& logger_section = config[logger_config_section_name_];

        if (logger_section.is_object())
        {
            load_logger_configuration(logger_section);
        }
    }

    //
    // Load the storage section if available.
    //
    if (config.contains(storage_config_section_name_))
    {
        const auto& storage_section = config[storage_config_section_name_];

        if (storage_section.is_object())
        {
            load_storage_configuration(storage_section);
        }
    }

    //
    // Load the server section if available.
    //
    if (config.contains(server_config_section_name_))
    {
        const auto& server_section = config[server_config_section_name_];

        if (server_section.is_object())
        {
            load_server_configuration(server_section);
        }
    }
}

void
system_configuration::set_up_system_directories()
{
    std::filesystem::create_directories(storage_configuration_.kv_store_path_);
    std::filesystem::create_directories(logger_configuration_.logs_directory_path_);
    std::filesystem::create_directories(server_configuration_.server_logs_directory_path_);
}

void
system_configuration::load_logger_configuration(
    const nlohmann::json& config_section)
{
    logger_configuration_.logs_directory_path_ =
        config_section.value("logs_directory_path", logger_configuration_.logs_directory_path_);

    logger_configuration_.component_name_ =
        config_section.value("component_name", logger_configuration_.component_name_);

    logger_configuration_.queue_size_bytes_ =
        config_section.value("queue_size_bytes", logger_configuration_.queue_size_bytes_);

    logger_configuration_.max_log_file_size_bytes_ =
        config_section.value("max_log_file_size_bytes", logger_configuration_.max_log_file_size_bytes_);

    logger_configuration_.max_number_files_for_session_ =
        config_section.value("max_number_files_for_session", logger_configuration_.max_number_files_for_session_);

    logger_configuration_.flush_frequency_ms_ =
        config_section.value("flush_frequency_ms", logger_configuration_.flush_frequency_ms_);

    logger_configuration_.log_file_prefix_ =
        config_section.value("log_file_prefix", logger_configuration_.log_file_prefix_);

    logger_configuration_.logging_session_directory_prefix_ =
        config_section.value("logging_session_directory_prefix", logger_configuration_.logging_session_directory_prefix_);
}

void
system_configuration::load_storage_configuration(
    const nlohmann::json& config_section)
{
    storage_configuration_.kv_store_path_ =
        config_section.value("kv_store_path", storage_configuration_.kv_store_path_);

    storage_configuration_.garbage_collector_periodic_interval_ms_ =
        config_section.value("garbage_collector_periodic_interval_ms", storage_configuration_.garbage_collector_periodic_interval_ms_);

    storage_configuration_.max_container_name_size_bytes_ =
        config_section.value("max_container_name_size_bytes", storage_configuration_.max_container_name_size_bytes_);

    storage_configuration_.max_object_id_size_bytes_ =
        config_section.value("max_object_id_size_bytes", storage_configuration_.max_object_id_size_bytes_);

    storage_configuration_.max_object_data_size_bytes_ =
        config_section.value("max_object_data_size_bytes", storage_configuration_.max_object_data_size_bytes_);

    storage_configuration_.max_number_containers_ =
        config_section.value("max_number_containers", storage_configuration_.max_number_containers_);

    storage_configuration_.number_write_io_threads_ =
        config_section.value("number_write_io_threads", storage_configuration_.number_write_io_threads_);

    storage_configuration_.number_read_io_threads_ =
        config_section.value("number_read_io_threads", storage_configuration_.number_read_io_threads_);

    storage_configuration_.storage_engine_block_cache_size_mib_ =
        config_section.value("storage_engine_block_cache_size_mib", storage_configuration_.storage_engine_block_cache_size_mib_);

    storage_configuration_.container_index_number_buckets_ =
        config_section.value("container_index_number_buckets", storage_configuration_.container_index_number_buckets_);

    storage_configuration_.number_frontline_cache_shards_ =
        config_section.value("number_frontline_cache_shards", storage_configuration_.number_frontline_cache_shards_);

    storage_configuration_.max_frontline_cache_shard_size_mib_ =
        config_section.value("max_frontline_cache_shard_size_mib", storage_configuration_.max_frontline_cache_shard_size_mib_);

    storage_configuration_.max_frontline_cache_shard_object_size_bytes =
        config_section.value("max_frontline_cache_shard_object_size_bytes", storage_configuration_.max_frontline_cache_shard_object_size_bytes);
}

void
system_configuration::load_server_configuration(
    const nlohmann::json& config_section)
{
    server_configuration_.port_number_ =
        config_section.value("port_number", server_configuration_.port_number_);

    server_configuration_.server_logs_directory_path_ =
        config_section.value("server_logs_directory_path", server_configuration_.server_logs_directory_path_);

    server_configuration_.server_number_threads_ =
        config_section.value("server_number_threads", server_configuration_.server_number_threads_);

    server_configuration_.server_listener_ip_address_ =
        config_section.value("server_listener_ip_address", server_configuration_.server_listener_ip_address_);
}

} // namespace common.
} // namespace lazarus.