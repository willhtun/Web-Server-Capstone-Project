#pragma once

#include <string>
#include <vector>
#include "request.h"
#include "response.h"
#include "request_handler.h"
#include "config_parser.h"

class ErrorHandler : public RequestHandler
{
public:     
    static RequestHandler*  create(const NginxConfig& config, const std::string& path);

    //RequestHandler::statuscode HandleRequest(Request request, Response& response);
    
    std::unique_ptr<Response> HandleRequest(const Request& request);

};