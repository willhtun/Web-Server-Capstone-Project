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
    static std::map<std::string,std::string> getMemeEntriesById(std::string id);

  private:
    static std::vector<std::map<std::string,std::string>> meme_entries_;
};