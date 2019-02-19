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
    
    std::unique_ptr<Response> HandleRequest(const Request& request);

private:
    std::string root_;
    std::string uri_;
};