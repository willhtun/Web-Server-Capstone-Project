#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <stdexcept>

class StatusObject
{
  public:
    static void addStatusEntry(std::string request_url, std::string resp_code);
    static std::vector<std::tuple<int,std::string,std::string>> getStatusEntries();
    static int getStatusCalls();
  
  private:
    static std::vector<std::tuple<int,std::string,std::string>> status_entries_;
};