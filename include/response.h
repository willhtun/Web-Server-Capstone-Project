#include <string>
#include <vector>

class Response 
{
    public: 
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

    void SetStatus(const statuscode status_code);

    void SetHeader(const std::string& header_name, const std::string& header_value);
    
    void SetBody(const std::string& body_value);



}