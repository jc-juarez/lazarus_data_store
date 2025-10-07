// ****************************************************
// Lazarus Data Store
// Common
// 'alertable_sleeper.hh'
// Author: jcjuarez
// Description:
//      Alertable sleeper class for stopping midway
//      sleep cycles.
// ****************************************************

#pragma once

#include <mutex>
#include <cstdint>
#include <stop_token>
#include <condition_variable>

namespace lazarus::common
{

class alertable_sleeper
{
public:

    //
    // Constructor.
    //
    alertable_sleeper();

    //
    // Sleep for a given number of milliseconds
    // or until stop is requested on the stop token.
    //
    bool wait_for_and_alert_if_stopped(
        std::stop_token stop_token,
        const std::uint32_t wait_time_milliseconds);

private:

    //
    // Mutex for the condition variable.
    //
    std::mutex lock_;

    //
    // Condition variable for the alertable logic.
    //
    std::condition_variable_any condition_variable_;
};

} // namespace lazarus::common.