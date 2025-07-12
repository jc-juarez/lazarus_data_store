// ****************************************************
// Lazarus Data Store
// Common
// 'task_serializer.hh'
// Author: jcjuarez
// Description:
//      Task serializer construct for single-threaded
//      purposes.
// ****************************************************

#pragma once

#include <boost/asio.hpp>

namespace lazarus::common
{

class task_serializer
{
public:

    //
    // Constructor.
    //
    task_serializer();

    //
    // Enqueues tasks into the serializer.
    //
    template<typename F>
    void enqueue_serialized_task(F&& task)
    {
        boost::asio::post(serializer_thread_pool_, std::forward<F>(task));
    }

private:

    //
    // Internal single-threaded thread pool for tasks dispatching.
    //
    boost::asio::thread_pool serializer_thread_pool_;
};

} // namespace lazarus::common.