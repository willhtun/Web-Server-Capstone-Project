#pragma once

#include <string>
#include <vector>

class StatusObject
{
  public:
    static void addStatusEntry(std::string request_url, std::string resp_code);
    static std::vector<std::pair<std::string,std::string>> getStatusEntries();
  
  private:
    static std::vector<std::pair<std::string,std::string>> status_entries_;
};