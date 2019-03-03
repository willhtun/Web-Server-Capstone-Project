#include <string>
#include <vector>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include "meme_handler.h"
#include <sstream> 
#include <fstream>



RequestHandler* MemeHandler::create(const NginxConfig& config, const std::string& path)
{
    std::cout << "Creating" << std::endl;
    MemeHandler* mh = new MemeHandler();
    mh->root_ = path;
    mh->filedir_ = config.GetAttribute("location");
    mh->uri_ = config.GetAttribute("url");

    BOOST_LOG_TRIVIAL(trace) << "Meme handler created";

    return mh;
};


std::unique_ptr<Response> MemeHandler::HandleRequest(const Request& request)
{
    //Meme landing page. Will probably need to move into another function once we figure out the uri path strategy 
    BOOST_LOG_TRIVIAL(trace) << "Meme handler building response for request...";

    std::string full_url = request.uri_path();
    memepage_ = full_url.substr(uri_.length() + 1, full_url.length());

    std::unique_ptr<Response> response(new Response());
    std::string memebody_;

    std::cout << "Processing request:\n" << request.getReqRaw() << std::endl;
    if (memepage_ == "create.html") //Landing Page for Meme Creation
    {
        std::cout << "Oh boi, we're creating that landing page" << std::endl;
        MemeCreate(move(response));
        std::cout << "What now.." << std::endl;

    }

    std::cout << "Finished with create.html..." << std::endl;
    if(memepage_ == "list.html") // List all created memes
    {
        MemeList();
    }
    
    /* TODO:: figure how how we want to generate and store IDs of memes created
    if(memepage_ == memeID)
    {
        MemeView();
    }
    */
    /*
    else //Send Error Response 
    {
        MemeError(move(response));
    }
    */
    //Send Response
    
    std::cout << "what's my error flag: " <<  errorflag << std::endl;
    if (!errorflag)
    {
        std::unique_ptr<Response> response(new Response());
        response->SetStatus(Response::OK);
        response->ReSetHeader("Content-Type","text/html");
        response->SetHeader("Content-Length", std::to_string(memebody_.length()));
        response->SetBody(memebody_);
    }   
    
    BOOST_LOG_TRIVIAL(trace) << "Response built by meme handler...";
    std::cout << "Finished creating response..." << std::endl;
    return response;
};

void MemeHandler::MemeCreate(std::unique_ptr<Response> response)
{
    //GCP uri_path
    //std::ifstream ifs("static" + filedir_ + "/" + memepage_, std::ios::in | std::ios::binary);
    //local uri_path
    std::cout << "Creating ifrstream..." << std::endl;
    std::ifstream ifs(".." + filedir_ + "/" + memepage_, std::ios::in | std::ios::binary);

    std::cout << "Creating that meme..." << std::endl;
    if (!ifs.is_open() || memepage_.length() == 0)
    {
        MemeError(move(response));
    }
    
    std::cout << "hopefully should be displaying content..." << std::endl;
    char buf[512];
    while (ifs.read(buf, sizeof(buf)).gcount() > 0) {
        memebody_.append(buf, ifs.gcount());
    }
    ifs.close();
    std::cout << "Closed ifstream..." << std::endl;
}

void MemeHandler::MemeView()
{
    /* From Assignment Example 
    std::string memebody_ = std::string(
        "<style>
        body { display: inline-block; position: relative; }
        span { color: white; font: 2em bold Impact, sans-serif; position: absolute; text-align: center; width: 100%; }
        #top { top: 0; }
        #bottom { bottom: 0; }
        </style>
        <body>                                                                                                                                         
        <img src="$meme.image">
        <span id="top">$meme.top</span>
        <span id="bottom">$meme.bottom</span>
        </body>"
    );
    */

}
void MemeHandler::MemeList()
{
    /* From Assignment Example 
    std::string body = std::string(
        for($meme in $all_memes)
            output <a href="/meme/view?id=$meme.id">
                $meme.id: $meme.image, $meme.top, $meme.bottom
         </a>
    );
    */

}

void MemeHandler::MemeError(std::unique_ptr<Response> response)
{
    std::string error_msg = "404: File not found on uri_path. Please provide correct uri_path.";
    response->SetStatus(Response::NOT_FOUND);
    response->ReSetHeader("Content-Type", "text/plain");
    response->SetHeader("Content-Length", std::to_string(error_msg.length()));
    response->SetBody(error_msg);
    errorflag = true;

}