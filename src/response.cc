#include <string>
#include <vector>

#include "response.h"


void Response::SetStatus(const statuscode status_code)
{
    status_code_ = status_code;
}
void Response::SetHeader(const std::string& header_name, const std::string& header_value)
{
    const std::string CRLF = "\r\n";
    header_ = header_name + ": " + header_value + CRLF; 
    //TODO: Handle multiple sections headers? 
    //This only handles one set of header_name:header_value right now but there may be multiple. 
}
void Response::SetBody(const std::string& body_value)
{
    body_ = body_value;
}

std::string Response::Output()
{   
    const std::string CRLF = "\r\n";

    //Build the full response message from pieces
    std::string status_line = "HTTP/1.1 " + std::to_string(status_code_) + CRLF; 
    //std::string header = full_header;
    //std::string message_body = body_value_;
    

    std::string full_reponse = status_line + header_ + CRLF + body_; 
    return full_reponse;

};

//testing something in the repo 

