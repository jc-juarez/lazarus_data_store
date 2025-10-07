// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
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