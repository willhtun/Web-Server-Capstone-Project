#include <string>
#include <vector>
#include <fstream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <sstream> 
#include "static_handler.h"
#include "server_object.h"
#include <iostream>

StaticHandler::StaticHandler(std::string filedir) {
    filedir_ = filedir;
}

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

    //read in file
    std::string image;
    std::ifstream ifs("static" + filedir_ + "/" + filename, std::ios::in | std::ios::binary);
   
   //if fail, give 404 error
   if (!ifs.is_open() || filename.length() == 0)
  {
    std::string error_msg = "404: File not found on path: usr/src/project";
    response.SetStatus(Response::NOT_FOUND);
    response.ReSetHeader("Content-Type", "text/plain");
    response.SetHeader("Content-Length", std::to_string(error_msg.length()));
    response.SetBody(error_msg + filedir_ + "/" + filename);
    return RequestHandler::NOT_FOUND;
  }
    char buf[512];
    while (ifs.read(buf, sizeof(buf)).gcount() > 0) {
        image.append(buf, ifs.gcount());
    }
    ifs.close();
    
    //text file MIMES
    if (fileextension == "html")
        contenttype = "text/html";
    else if (fileextension == "htm")
        contenttype = "text/html";
    else if (fileextension == "css")
        contenttype = "text/css";
    else if (fileextension == "csv")
        contenttype = "text/csv";
    else if (fileextension == "txt")
        contenttype = "text/plain";
    else if (fileextension == "js")
        contenttype = "text/javascript";
    else if (fileextension == "xml")
        contenttype = "text/xml";

    //image file MIMES
    else if (fileextension == "jpg" || fileextension == "jpeg")
        contenttype = "image/jpeg";
    else if (fileextension == "png")
        contenttype = "image/png";
    else if (fileextension == "gif")
        contenttype = "image/gif";
    else if (fileextension == "tif" || fileextension == "tiff")
        contenttype = "image/tiff";

    //application file MIMES
    else if (fileextension == "pdf")
        contenttype = "application/pdf";
    else if (fileextension == "zip")
        contenttype = "application/zip";
    else if (fileextension == "rtf")
        contenttype = "application/rtf";
    else if (fileextension == "json")
        contenttype = "application/json";
    else if (fileextension == "doc")
        contenttype = "application/msword";

    else
    {
        response.SetStatus(Response::UNSUPPORTED_MEDIA_TYPE);
        response.ReSetHeader("Content-Type", "text/plain");
        response.SetHeader("Content-Length", std::to_string(27));
        response.SetBody("415: UNSUPPORTED_MEDIA_TYPE");
        return RequestHandler::UNSUPPORTED_MEDIA_TYPE;
    }

    //send a correct response 
    response.SetStatus(Response::OK);
    response.ReSetHeader("Content-Type", contenttype);
    response.SetHeader("Content-Length", std::to_string(image.length()));
    response.SetBody(image);
    BOOST_LOG_TRIVIAL(trace) << "Response built by static handler...";
    
    return RequestHandler::OK;
};