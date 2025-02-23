#include "lazarus_data_store.hh"

int main()
{
   lazarus::lazarus_data_store lazarus_ds{
      lazarus::logger::logger_configuration{},
      lazarus::network::server_configuration{}};

   lazarus_ds.start_system();
}