// ****************************************************
// Lazarus Data Store
// Common
// 'aliases.hh'
// Author: jcjuarez
// Description:
//      Provides aliases definitions. 
// ****************************************************

#pragma once

namespace lazarus
{

//
// lazarus namespace.
//
using exit_code = std::int32_t;

//
// network namespace.
//
namespace network
{
using server_response_callback = std::function<void(const drogon::HttpResponsePtr&)>;
}


} // namespace lazarus.