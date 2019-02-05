#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "response.h"

Response::Response() {
    header_ = "";
}

void Response::SetStatus(const statuscode status_code)
{
    status_code_ = status_code;
}
void Response::SetHeader(const std::string& header_name, const std::string& header_value)
{
    const std::string CRLF = "\r\n";
    header_ += header_name + ": " + header_value + CRLF; 
}
void Response::SetBody(char* body_value, int body_size)
{
    body_ = body_value;
    bodysize_ = body_size;
}

char* Response::Output()
{   
    const std::string CRLF = "\r\n";

    //Build the full response message from pieces
    std::string status_line = "HTTP/1.1 " + std::to_string(status_code_) + CRLF; 
    
    char* full_response = new char [status_line.length() + header_.length() + bodysize_ + CRLF.length()];
    statuslinesize_ = status_line.length();
    headersize_ = header_.length() + CRLF.length(); // needed for output

    //strcpy(full_response, status_line.c_str());
    //strcat(full_response, header_.c_str());
    //strcat(full_response, CRLF.c_str());
    strcpy(full_response, body_);

    std::cout << "HERE IS OUTPUT=========================" << std::endl;
    for (int i = 0; i < bodysize_; i++) {
        std::cout << body_[i]; //fullresponse[i]
    }
    return full_response;

};

int Response::Size() {
    return  bodysize_ + headersize_ + statuslinesize_;
}