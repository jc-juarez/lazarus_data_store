// ****************************************************
// Lazarus Data Store
// Common
// 'task_serializer.cc'
// Author: jcjuarez
// Description:
//      Task serializer construct for single-threaded
//      purposes.
// ****************************************************

#include "task_serializer.hh"

namespace lazarus::common
{

task_serializer::task_serializer()
    : serializer_thread_pool_{1u} // Must always be a single-threaded task queue.
{}

} // namespace lazarus::common.