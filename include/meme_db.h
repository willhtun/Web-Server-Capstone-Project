#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <map>

class MemeDB
{
  public:
    static void addMemeEntry(std::map<std::string,std::string> meme_entry);
    static std::vector<std::map<std::string,std::string>> getMemeEntries();
 
  private:
    static std::vector<std::map<std::string,std::string>> meme_entries_;
};