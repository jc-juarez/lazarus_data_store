// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'collocation_provider.hh'
// Author: jcjuarez
// Description:
//      Provides access to a specific collocation
//      subtype.
// ****************************************************

#pragma once

#include <memory>

namespace lazarus
{
namespace storage
{

class task_router;

template<class T>
class collocation_provider
{
public:

    //
    // Constructor.
    //
    collocation_provider(
        std::shared_ptr<task_router> task_router);

    //
    // Returns the appropriate collocation subtype tied to the object key.
    //
    std::shared_ptr<T>
    get_collocation_subtype();

private:

    //
    // Task router reference.
    //
    std::shared_ptr<task_router> task_router_;

    //
    // Collocation subtype table reference.
    //
    std::shared_ptr<collocation_table<T>> collocation_table_;
};

} // namespace storage.
} // namespace lazarus.