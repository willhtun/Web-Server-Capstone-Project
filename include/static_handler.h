#pragma once

#include <string>
#include <vector>
#include "request.h"
#include "response.h"
#include "request_handler.h"

class StaticHandler : public RequestHandler
{
public:
    // this is still fine to keep
    StaticHandler(std::string filedir);
    // change handling request function to return a Response object
    virtual RequestHandler::statuscode HandleRequest(Request request, Response& response);
private:
    std::string filedir_;
};