#include <string>
#include <vector>
#include "echo_handler.h"

RequestHandler::statuscode HandleRequest(Request request, Response response)
{

    response.SetStatus(Response::OK);
    response.SetHeader("Content-Type","text/plain");
    response.SetBody(std::string(request.raw_req));

    return RequestHandler::OK;

};