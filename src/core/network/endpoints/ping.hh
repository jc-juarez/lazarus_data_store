// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Network
// 'ping.hh'
// Author: jcjuarez
// Description:
//      Ping service endpoint for liveliness probes.
// ****************************************************

#pragma once

#include "../../common/aliases.hh"
#include <drogon/HttpController.h>

namespace lazarus
{
namespace network
{
namespace endpoints
{

class ping : public drogon::HttpController<ping, false>
{
public:

    //
    // Endpoint constructor.
    //
    ping() = default;

    METHOD_LIST_BEGIN
        METHOD_ADD(ping::reply_to_ping, "", drogon::Get);
    METHOD_LIST_END

    //
    // Liveliness probe method.
    //
    void
    reply_to_ping(
        const http_request& request,
        server_response_callback&& response_callback);
};

} // namespace endpoints.
} // namespace network.
} // namespace lazarus.