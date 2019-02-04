#include <string>
#include <vector>
#include "echo_handler.h"

RequestHandler::statuscode EchoHandler::HandleRequest(Request request, Response response)
{

    response.SetStatus(Response::OK);
    response.SetHeader("Content-Type","text/plain");
    response.SetBody(std::string(request.getReqRaw()));

    return RequestHandler::OK;

};