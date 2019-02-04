#include <string>
#include <vector>
#include <iostream>
#include "static_handler.h"

RequestHandler::statuscode StaticHandler::HandleRequest(Request request, Response response)
{
    // uri: /static/somefile.html
    std::string filename = (request.uri_path()).substr(8, request.uri_path().length());
    std::cout << filename;
    response.SetStatus(Response::OK);
    response.SetHeader("Content-Type","text/html");
    response.SetBody(std::string(request.getReqRaw()));

    return RequestHandler::OK;

};