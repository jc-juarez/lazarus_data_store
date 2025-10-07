// ****************************************************
// Lazarus Data Store
// Common
// 'alertable_sleeper.hh'
// Author: jcjuarez
// Description:
//      Alertable sleeper class for stopping midway
//      sleep cycles.
// ****************************************************

#include <chrono>
#include "alertable_sleeper.hh"

namespace lazarus::common
{

alertable_sleeper::alertable_sleeper()
{}

bool
alertable_sleeper::wait_for_and_alert_if_stopped(
    std::stop_token stop_token,
    const std::uint32_t wait_time_milliseconds)
{
    std::unique_lock<std::mutex> lock {lock_};
    return condition_variable_.wait_for(
        lock,
        stop_token,
        std::chrono::milliseconds(wait_time_milliseconds),
        [&stop_token] { return stop_token.stop_requested(); });
}

} // namespace lazarus::common.