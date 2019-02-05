#include <string>
#include <vector>
#include <fstream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include "static_handler.h"
#include "server_object.h"

RequestHandler::statuscode StaticHandler::HandleRequest(Request request, Response& response)
{
    // uri: /static/somefile.html
    BOOST_LOG_TRIVIAL(trace) << "Static Handler building response for request...";
    std::string filename = (request.uri_path()).substr(8, request.uri_path().length());

    std::ifstream ifs(".." + ServerObject::staticfile_dir + "/" + filename);
    std::string filecontent( (std::istreambuf_iterator<char>(ifs) ),
                            (std::istreambuf_iterator<char>()    ) );

    response.SetStatus(Response::OK);
    response.SetHeader("Content-Type","text/html");
    response.SetHeader("Content-Length", std::to_string(filecontent.length()));
    response.SetBody(filecontent);
    BOOST_LOG_TRIVIAL(trace) << "Response built...";
    
    return RequestHandler::OK;

};