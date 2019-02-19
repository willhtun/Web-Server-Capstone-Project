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
    // create fresh response obj
    BOOST_LOG_TRIVIAL(trace) << "Status handler building response for request...";
    std::unique_ptr<Response> response(new Response());

    std::string body = CreateBody();

    // set response data
    response->SetStatus(Response::OK);
    response->ReSetHeader("Content-Type","text/plain");
    response->SetHeader("Content-Length", std::to_string(body.length()));
    response->SetBody(body);
    BOOST_LOG_TRIVIAL(trace) << "Response built by Status Handler...";

    return response;
};

std::string StatusHandler::CreateBody()
{
    // get status entry database
    std::vector<std::pair<std::string,std::string>> status_entries = config_.getStatusObject().getStatusEntries();

    // build body string
    std::string body = "";

    // bring in request url and response code pairs
    for (std::vector<std::pair<std::string,std::string>>::const_iterator it = status_entries.begin();
         it != status_entries.end(); it++)
    {
        body += "Request Url: " + it->first + " | Response Code: " + it->second + "\n";
    }

    // bring in handler details
    body += "\nRequest Handlers which Exist:\n";
    std::vector<std::string> myReqHandlers = config_.getReqHandlers();
    for (std::vector<std::string>::const_iterator it = myReqHandlers.cbegin(); it != myReqHandlers.cend(); it++)
    {
        // TODO: specify handler and corresponding prefix
        body += *it + "\n";
    }

    return body;    
};