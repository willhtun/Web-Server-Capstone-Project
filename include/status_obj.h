#pragma once

#include <string>
#include <vector>

class StatusObject
{
  public:
    void addStatusEntry(std::string request_url, std::string resp_code);
    std::vector<std::pair<std::string,std::string>> getStatusEntries();
  
  private:
    std::vector<std::pair<std::string,std::string>> status_entries_;
};