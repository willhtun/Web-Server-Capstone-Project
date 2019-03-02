#include <string>
#include <vector>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include "meme_handler.h"


RequestHandler* MemeHandler::create(const NginxConfig& config, const std::string& path)
{
    EchoHandler* eh = new EchoHandler();
    eh->root_ = path;
    eh->uri_ = config.GetAttribute("url");

    BOOST_LOG_TRIVIAL(trace) << "Echo handler created";

    return eh;
};


std::unique_ptr<Response> MemeHandler::HandleRequest(const Request& request)
{
    //Meme landing page. Will probably need to move into another function once we figure out the uri path strategy 
    BOOST_LOG_TRIVIAL(trace) << "Meme handler building response for request...";

    std::unique_ptr<Response> response(new Response());
    std::string body_ = std::string(
        "<form action="/meme/create" method="post">
        <select name="image">
        <option>Select a template...</option>
        <option value="simply.jpg">one does not simply</option>
        <option value="grumpy.jpg">grumpy cat</option>
        </select><br>
        <input type="text" name="top" placeholder="Top text..."><br>
        <input type="text" name="bottom" placeholder="Bottom text..."><br>
        <input type="submit" value="Create">
        </form>"
);

    response->SetStatus(Response::OK);
    response->ReSetHeader("Content-Type","text/html");
    response->SetHeader("Content-Length", std::to_string(body_.length()));
    response->SetBody(body_);
    
    BOOST_LOG_TRIVIAL(trace) << "Response built by meme handler...";

    return response;
};