#pragma once

#include <string>
#include <vector>
#include "request.h"
#include "response.h"
#include "request_handler.h"

class EchoHandler : public RequestHandler
{
public:     
    //this is old handlerequest - remove when necessary 
    //RequestHandler::statuscode HandleRequest(Request request, Response& response);


    //create an instance of the handler
    static RequestHandler*  create(const NginxConfig& config, const std::string& path);

    //Handles the request and generates a response
    std::unique_ptr<Response> HandleRequest(const Request& request);

};