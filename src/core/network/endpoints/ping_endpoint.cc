// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Network
// 'ping_endpoint.cc'
// Author: jcjuarez
// Description:
//     Ping service endpoint for liveliness probes.
// ****************************************************

#include "ping_endpoint.hh"
#include "../server/server.hh"

namespace lazarus
{
namespace network
{

void
ping_endpoint::ping(
    const http_request& request,
    server_response_callback&& response_callback)
{
    network::server::send_response(
        response_callback,
        status::success);
}

} // namespace network.
} // namespace lazarus.