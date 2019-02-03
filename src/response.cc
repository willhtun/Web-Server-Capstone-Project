#include <string>
#include <vector>

#include "reponse.h"


void Response::SetStatus(const statuscode status_code)
{
    status_code_ = status_code;
}
void Response::SetHeader(const std::string& header_name, const std::string& header_value)
{
    //TODO: Handle splicing the header onto the message 
}
void Response::SetBody(const std::string& body_value)
{
    body_value_ = body_value
}

//TODO: Convert this response to a string to output message 
