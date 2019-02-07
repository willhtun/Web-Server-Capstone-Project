#include <string>
#include <vector>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include "echo_handler.h"

RequestHandler::statuscode EchoHandler::HandleRequest(Request request, Response& response)
{
    BOOST_LOG_TRIVIAL(trace) << "Echo handler building response for request...";
    std::string body_ = std::string(request.getReqRaw());

    response.SetStatus(Response::OK);
    response.ReSetHeader("Content-Type","text/plain");
    response.SetHeader("Content-Length", std::to_string(body_.length()));
    response.SetBody(body_);
    BOOST_LOG_TRIVIAL(trace) << "Response built by echo handler...";

    return RequestHandler::OK;

};