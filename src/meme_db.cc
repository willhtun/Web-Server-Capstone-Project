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

std::map<std::string,std::string> MemeDB::getMemeEntriesById(std::string id) 
{
   for (int i = 0; i < meme_entries_.size(); i++) {
      if ((meme_entries_[i])["meme-id"] == id)
         return meme_entries_[i];
   }
   // Not returning anything for not found
}

std::vector<std::map<std::string,std::string>> MemeDB::meme_entries_;
