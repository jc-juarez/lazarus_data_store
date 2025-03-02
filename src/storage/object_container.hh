// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container.hh'
// Author: jcjuarez
// Description:
//      Object container metadata structure. Object
//      containers are logical buckets for holding
//      closely related data inside the data store. 
// ****************************************************

#pragma once

#include <string>
#include "object_container_persistance_interface.pb.h"

namespace lazarus
{
namespace storage
{

//
// Object container metadata internal structure.
// An in-memory creation of an object container reference can
// only be created after a well-known committed disk write its creation.
//
class object_container
{
public:

    //
    // Constructor for the object container.
    //
    object_container(
        const lazarus::schemas::object_container_persistance_interface& object_container_persistance);

private:

    //
    // Object container identifier.
    //
    const std::string id_;
};

} // namespace storage.
} // namespace lazarus.