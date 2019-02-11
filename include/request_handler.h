#pragma once

#include <string>
#include <vector>
#include "request.h"
#include "response.h"

class Request;
class Response;

class RequestHandler
{
public: 
    /*
        Major TODO: get rid of status code and add the following functions
    */
    enum statuscode
    {
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NO_CONTENT = 204,
        MULTIPLE_CHOICES = 300,
        MOVED_PERMANENTLY = 301,
        MOVED_TEMPORARILY = 302,
        NOT_MODIFIED = 304,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        UNSUPPORTED_MEDIA_TYPE = 415,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503
    };
    virtual statuscode HandleRequest(Request request, Response& response) = 0;
    // Response gather_response(Request request);
/*
private:
    Response dispatch(std::string uri_type); // this function will allow us to create specialized request handling objects
    std::string parse_uri(std::string uri_path);
*/
};