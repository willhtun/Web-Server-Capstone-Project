#include "status_obj.h"

void StatusObject::addStatusEntry(std::string request_url, std::string resp_code)
{
    /*
        Adds a request url and response code to our "Status Database"

        Author: Konner Macias
    */
   std::pair<std::string,std::string> status_entry(request_url, resp_code);
   status_entries_.push_back(status_entry);
}

std::vector<std::pair<std::string,std::string>> StatusObject::getStatusEntries()
{
    /*
        Author: Konner Macias
    */
   return status_entries_;
}

std::vector<std::pair<std::string,std::string>> StatusObject::status_entries_;