#pragma once

#include <string>
#include <vector>
#include "request.h"
#include "response.h"
#include "config_parser.h"

class Request;
class Response;

class RequestHandler
{
public: 
    /* Commented out as we refactor. Delete as necessary
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
    */

    static RequestHandler* create(const NginxConfig& config, const std::string& path);

    virtual  std::unique_ptr<Response> HandleRequest(const Request& request);

    protected: 

    //what do we actually need here?
    std::string path_;
    NginxConfig config_;


    Request req;
    Response response;






    //virtual statuscode HandleRequest(Request request, Response& response) = 0;
    // Response gather_response(Request request);
/*
private:
    Response dispatch(std::string uri_type); // this function will allow us to create specialized request handling objects
    std::string parse_uri(std::string uri_path);
*/
};