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

#include <tbb/tbb.h>
#include <functional>

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
        serializer_queue_.enqueue(std::forward<F>(task));
    }

private:

    //
    // Internal task arena queue for handling tasks dispatching.
    //
    tbb::task_arena serializer_queue_;
};

} // namespace lazarus::common.