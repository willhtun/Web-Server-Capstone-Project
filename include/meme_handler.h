#pragma once

#include <string>
#include <vector>
#include <sqlite3.h>
#include <chrono>
#include <ctime>    
#include "request.h"
#include "response.h"
#include "request_handler.h"

class MemeHandler : public RequestHandler
{
public:     

    //create an instance of the handler
    static RequestHandler* create(const NginxConfig& config, const std::string& path);

    //Handles the request and generates a response
    std::unique_ptr<Response> HandleRequest(const Request& request);

private:
    bool MemeCreate();
    bool MemeView();
    bool MemeList();
    std::map<std::string,std::string> parseRequestBody(std::string body);
    
    sqlite3 *db;
    bool AddToDatabase(std::string id_, std::string image_, std::string top_, std::string bottom);
    std::vector<std::map<std::string,std::string>> GetAllFromDatabase();

    std::string root_;
    std::string uri_;
    std::string filedir_;
    std::string savedir_;
    std::string memepage_;
    std::string memebody_;
    bool errorflag = false;
};