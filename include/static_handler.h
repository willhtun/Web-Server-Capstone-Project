#pragma once

#include <string>
#include <vector>
#include "request.h"
#include "response.h"
#include "request_handler.h"

class StaticHandler : public RequestHandler
{
public: 
    virtual RequestHandler::statuscode HandleRequest(Request request, Response& response);
private:
    std::string filedir_;
};