// ****************************************************
// Lazarus Data Store
// Main
// 'lazarus_data_store.hh'
// Author: jcjuarez
// Description:
//      Lazarus Data Store root object. 
// ****************************************************

#pragma once

#include <memory>

namespace lazarus
{

class server;

//
// Lazarus system root object.
//
class lazarus_data_store
{
public:

    //
    // Constructor.
    //
    lazarus_data_store();

    //
    // Start the lazarus data store system.
    //
    void
    start_system();

private:

    //
    // HTTP server handle.
    //
    std::shared_ptr<server> server_;
};

} // namespace lazarus.
