// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Network
// 'logging_context_filter.hh'
// Author: jcjuarez
// Description:
//      Provided the logging context for network
//      data requests.
// ****************************************************

#pragma once

#include <drogon/HttpFilter.h>

namespace pandora
{
namespace network
{

class logging_context_filter : public drogon::HttpFilter<logging_context_filter, false>
{
public:

    //
    // Constructor.
    //
    logging_context_filter() = default;

    //
    // Applies the logging context for network requests.
    //
    void
    doFilter(
        const drogon::HttpRequestPtr& http_request,
        drogon::FilterCallback&& filter_callback,
        drogon::FilterChainCallback&& filter_chain_callback) override;
};

} // namespace network.
} // namespace pandora.