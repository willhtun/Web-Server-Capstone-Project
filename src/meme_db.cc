#include "meme_db.h"


void MemeDB::addMemeEntry(std::map<std::string,std::string> meme_entry)
{
   meme_entries_.push_back(meme_entry);
   return;
}

std::vector<std::map<std::string,std::string>> MemeDB::getMemeEntries()
{
  
   return meme_entries_;
}

std::vector<std::map<std::string,std::string>> MemeDB::meme_entries_;