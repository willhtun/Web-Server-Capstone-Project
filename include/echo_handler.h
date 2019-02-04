#include <string>
#include <vector>

#include "request_handler.h"

class EchoHandler : public RequestHandler
{
public: 

virtual RequestHandler::statuscode HandleRequest(Request request, Response response);

private:

};