#include <string>
#include "server_object.h"

int ServerObject::port = 0;
std::vector <std::string> ServerObject::staticfile_dir;
std::vector <std::string> ServerObject::staticfile_url;

int ServerObject::findStaticDir(std::string key) {
    for (int i = 0; i < staticfile_dir.size(); i++) {
        if (staticfile_url[i] == key)
            return i;
    }
    return -1;
}