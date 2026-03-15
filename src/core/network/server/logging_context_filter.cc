// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Network
// 'logging_context_filter.cc'
// Author: jcjuarez
// Description:
//      Provided the logging context for network
//      data requests.
// ****************************************************

#include "../../logger/logging.hh"
#include "logging_context_filter.hh"
#include "../../common/uuid_utilities.hh"

namespace pandora
{
namespace network
{

void
logging_context_filter::doFilter(
    const drogon::HttpRequestPtr& http_request,
    drogon::FilterCallback&& filter_callback,
    drogon::FilterChainCallback&& filter_chain_callback)
{
    //
    // Apply request logging context.
    //
    logger::context::request_id.clear();
    logger::context::request_id = common::uuid_to_string(common::generate_uuid());

    //
    // Execute endpoint.
    //
    filter_chain_callback();
}

} // namespace network.
} // namespace pandora.