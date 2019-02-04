#pragma once

#include <string>
#include <vector>
#include "request.h"
#include "response.h"
#include "request_handler.h"

class EchoHandler : public RequestHandler
{
public: 

RequestHandler::statuscode HandleRequest(Request request, Response response);

private:

};