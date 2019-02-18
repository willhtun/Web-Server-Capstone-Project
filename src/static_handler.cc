#include <string>
#include <vector>
#include <fstream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <sstream> 
#include "static_handler.h"
#include "server_object.h"
#include <iostream>


static RequestHandler* create(const NginxConfig& config, const std::string& uri_path)
{
    // leave blank for now
}

std::string StaticHandler::parse_uri(std::string uri_path)
{
    // we have no access to a request object, we only have the uri_path which is helpful. I'm guessing uri_path is uri_path
    int uri_length = 1;
    while (uri_length < uri_path.length() && uri_path[uri_length] != '/') 
    {
        uri_length++;
    }
    
    // set uri_type
    std::string uri_type = uri_path.substr(1, uri_length - 1);

    // determine static directory
    int dir_id = ServerObject::findStaticDir(uri_type);
    assert(dir_id != -1);

    return ServerObject::staticfile_dir[dir_id];
}

std::unique_ptr<Response> StaticHandler::HandleRequest(const Request& request)
{
    // uri: /static/somefile.html
    BOOST_LOG_TRIVIAL(trace) << "Static Handler building response for request...";
    std::string filename = (request.uri_path()).substr(8, request.uri_path().length());
    std::string fileextension;
    std::string contenttype;

    int dot_index = filename.find(".");
    if (dot_index != std::string::npos)
        fileextension = filename.substr(dot_index + 1, filename.length() - dot_index - 1);

    // determine file directory
    std::string filedir_ = parse_uri(request.uri_path());

    //read in file
    std::string image;
    std::ifstream ifs("static" + filedir_ + "/" + filename, std::ios::in | std::ios::binary);
     // local uri_path
    //std::ifstream ifs(".." + filedir_ + "/" + filename, std::ios::in | std::ios::binary);
   
    //if fail, give 404 error
    std::unique_ptr<Response> response(new Response());
    //TODO: replace with error_handler
    if (!ifs.is_open() || filename.length() == 0)
    {
        std::string error_msg = "404: File not found on uri_path. Please provide correct uri_path.";
        response->SetStatus(Response::NOT_FOUND);
        response->ReSetHeader("Content-Type", "text/plain");
        response->SetHeader("Content-Length", std::to_string(error_msg.length()));
        response->SetBody(error_msg);
    }
    char buf[512];
    while (ifs.read(buf, sizeof(buf)).gcount() > 0) {
        image.append(buf, ifs.gcount());
    }
    ifs.close();
    
    //TODO: Covert MIME handling to map
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
    //TODO: Replace with error_handler
    { 
        std::string error_msg = "404: File not found on uri_path. Please provide correct uri_path.";
        response->SetStatus(Response::NOT_FOUND);
        response->ReSetHeader("Content-Type", "text/plain");
        response->SetHeader("Content-Length", std::to_string(error_msg.length()));
        response->SetBody(error_msg);
        return response;
    }

    //send a correct response 
    response->SetStatus(Response::OK);
    response->SetHeader("Content-Type", contenttype);
    response->SetHeader("Content-Length", std::to_string(image.length()));
    response->SetBody(image);
    BOOST_LOG_TRIVIAL(trace) << "Response built by static handler...";
    
    return response;
};