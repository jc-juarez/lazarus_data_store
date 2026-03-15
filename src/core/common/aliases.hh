// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Common
// 'aliases.hh'
// Author: jcjuarez
// Description:
//      Provides aliases definitions. 
// ****************************************************

#pragma once

#include <cstdint>
#include <functional>
#include <rocksdb/db.h>
#include <drogon/drogon.h>
#include "../logger/logging.hh"

namespace pandora
{

//
// pandora namespace.
//
using exit_code = std::int32_t;

//
// network namespace.
//
namespace network
{
using server_response_callback = std::function<void(const drogon::HttpResponsePtr&)>;
using response_fields = std::unordered_map<const char*, std::string*>;
using http_request = drogon::HttpRequestPtr;
}

//
// storage namespace.
//
namespace storage
{
using byte = char;
using byte_stream = std::string;
using storage_engine_reference = rocksdb::ColumnFamilyHandle;
using storage_engine_write_batch = rocksdb::WriteBatch;
}

} // namespace pandora.