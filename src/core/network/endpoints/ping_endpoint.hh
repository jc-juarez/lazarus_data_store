// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Network
// 'ping_endpoint.hh'
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

class ping_endpoint : public drogon::HttpController<ping_endpoint, false>
{
public:

    //
    // Endpoint constructor.
    //
    ping_endpoint() = default;

    METHOD_LIST_BEGIN
        METHOD_ADD(ping_endpoint::ping, "/", drogon::Get);
    METHOD_LIST_END

    //
    // Liveliness probe method.
    //
    void
    ping(
        const http_request& request,
        server_response_callback&& response_callback);
};

} // namespace network.
} // namespace lazarus.