#include <string>
#include <vector>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include "status_handler.h"

RequestHandler* StatusHandler::create(const NginxConfig& config, const std::string& path)
{
    //creates an instance of status   
};

std::unique_ptr<Response> StatusHandler::HandleRequest(const Request& request)
{
    BOOST_LOG_TRIVIAL(trace) << "Status handler building response for request...";

    std::unique_ptr<Response> response(new Response());

    std::string status_body = StatusHandler::CreateBody();

    response->SetStatus(Response::OK);
    response->ReSetHeader("Content-Type", "text/plain");
    response->SetHeader("Content-Length", std::to_string(status_body.length()));
    response->SetBody(status_body);
    
    BOOST_LOG_TRIVIAL(trace) << "Response built by status handler...";

    return response;
};

std::string StatusHandler::CreateBody()
{
    //some method to generate the full status report from the ServerStatus object 
    //probably want to http or many CRLF format it so that it doesn't come out in a huge blog. 
};