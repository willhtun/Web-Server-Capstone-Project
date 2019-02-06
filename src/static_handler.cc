#include <string>
#include <vector>
#include <fstream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <sstream> 
#include "static_handler.h"
#include "server_object.h"
#include <iostream>

RequestHandler::statuscode StaticHandler::HandleRequest(Request request, Response& response)
{
    // uri: /static/somefile.html
    BOOST_LOG_TRIVIAL(trace) << "Static Handler building response for request...";
    std::string filename = (request.uri_path()).substr(8, request.uri_path().length());
    std::string fileextension;
    std::string contenttype;

    int dot_index = filename.find(".");
    if (dot_index != std::string::npos)
        fileextension = filename.substr(dot_index + 1, filename.length() - dot_index - 1);

    std::string image;
    std::ifstream ifs(".." + ServerObject::staticfile_dir + "/" + filename, std::ios::in | std::ios::binary);
   
    char buf[512];
    while (ifs.read(buf, sizeof(buf)).gcount() > 0) {
        image.append(buf, ifs.gcount());
    }

    ifs.close();

    response.SetStatus(Response::OK);

    if (fileextension == "html")
        contenttype = "text/html";
    else if (fileextension == "css")
        contenttype = "text/css";
    else if (fileextension == "jpg" || fileextension == "jpeg")
        contenttype = "image/jpeg";
    else if (fileextension == "png")
        contenttype = "image/png";
    else if (fileextension == "gif")
        contenttype = "image/gif";
    else if (fileextension == "pdf")
        contenttype = "application/pdf";
    else if (fileextension == "zip")
        contenttype = "application/zip";
    else
        contenttype = "text/plain";
    
    response.SetHeader("Content-Type", contenttype);
    response.SetHeader("Content-Length", std::to_string(image.length()));
    response.SetBody(image);
    
    return RequestHandler::OK;
};