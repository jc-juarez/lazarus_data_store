// ****************************************************
// Lazarus Data Store
// Status
// 'status.hh'
// Author: jcjuarez
// Description:
//      Status code structure
//      for holding errors metadata.
// ****************************************************

#pragma once

#include <cstdint>

namespace lazarus::status
{

struct status_code
{
    //
    // Constructor.
    //
    constexpr
    status_code(
        const std::uint32_t internal_status_code,
        const std::uint32_t http_status_code,
        const char* name)
        : internal_status_code_{internal_status_code},
          http_status_code_{http_status_code},
          name_{name}
    {}

    //
    // Cast operator for direct comparisons.
    //
    constexpr
    operator std::uint32_t() const
    {
        return internal_status_code_;
    }

    //
    // Gets the internal status code.
    //
    std::uint32_t
    get_internal_status_code() const
    {
        return internal_status_code_;
    }

    //
    // Gets the http status code.
    //
    std::uint32_t
    get_http_status_code() const
    {
        return http_status_code_;
    }

private:

    //
    // Internal status code number.
    //
    std::uint32_t internal_status_code_;

    //
    // Associated HTTP status code number.
    //
    std::uint32_t http_status_code_;

    //
    // Name of the status code.
    // Must be associated to a string literal with program-lifetime.
    //
    const char* name_;
};

} // namespace lazarus::status.