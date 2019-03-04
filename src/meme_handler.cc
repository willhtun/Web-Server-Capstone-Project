#include <string>
#include <vector>
#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include "meme_handler.h"
#include <sstream>
#include "meme_db.h"
#include <sstream> 
#include <fstream>
#include <thread>
#include <mutex>
#include <string>

std::mutex mtx;

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
    BOOST_LOG_TRIVIAL(trace) << "Meme handler building response for request...";
    std::string full_url = request.uri_path();
    memepage_ = full_url.substr(uri_.length() + 1, full_url.length());

    std::cout << "Processing request:\n" << request.getReqRaw() << std::endl;
    if (memepage_ == "create") //Landing Page for Meme Creation
    {
        errorflag = MemeCreate();
    }

    std::cout << "Finished with create.html..." << std::endl;
    if(memepage_ == "list") // List all created memes
    {
        errorflag = MemeList();
    }

    if(memepage_.substr(0,4) == "view") // Page to view a meme
    {
        errorflag = MemeView();
    }
    
    if (request.method() == "POST") {
        std::map<std::string,std::string> memeMap = parseRequestBody(request.body());

        // add meme-id with correct locks in place
        mtx.lock();
        memeMap["meme-id"] = std::to_string(MemeDB::getMemeEntries().size() + 1);
        mtx.unlock();

        // add meme to memeDB
        MemeDB::addMemeEntry(memeMap);
        for(std::map<std::string,std::string>::const_iterator it = memeMap.begin();
                it != memeMap.end(); ++it)
        {
            std::cout << "Key: " << it->first << " Value: " << it->second << std::endl;
        }
    }
    else 
    {
        errorflag = true; 
    }
    
    std::unique_ptr<Response> response(new Response());

    //Send Correct Response
    if (!errorflag)
    {
        response->SetStatus(Response::OK);
        response->ReSetHeader("Content-Type","text/html");
        response->SetHeader("Content-Length", std::to_string(memebody_.length()));
        response->SetBody(memebody_);
    }  
    else //Send Error Response
    {
        std::string error_msg = "404: File not found on uri_path. Please provide correct uri_path.";
        response->SetStatus(Response::NOT_FOUND);
        response->ReSetHeader("Content-Type", "text/plain");
        response->SetHeader("Content-Length", std::to_string(error_msg.length()));
        response->SetBody(error_msg);
    }
    
    
    BOOST_LOG_TRIVIAL(trace) << "Response built by meme handler...";
    std::cout << "Finished creating response..." << std::endl;
    std::cout << "Response:\n" << response->Output() << std::endl;
    return response;
};

bool MemeHandler::MemeCreate()
{
    /*
        Hosts create.html page. Returns false if we achieved no error.
    */
    std::cout << "Creating ifrstream..." << std::endl;
    std::ifstream ifs(".." + filedir_ + "/" + memepage_ + ".html", std::ios::in | std::ios::binary);

    std::cout << "Creating that meme..." << std::endl;
    if (!ifs.is_open() || memepage_.length() == 0)
    {
        return true;
    }
    
    std::cout << "hopefully should be displaying content..." << std::endl;
    char buf[512];
    while (ifs.read(buf, sizeof(buf)).gcount() > 0) 
    {
        memebody_.append(buf, ifs.gcount());
    }
    ifs.close();
    std::cout << "Closed ifstream..." << std::endl;
    return false;
}

std::map<std::string,std::string> MemeHandler::parseRequestBody(std::string body)
{
    /*
        Parses request body and assigns appropriate key value pairs inside
        of a meme map which is returned.

        Author: Konner Macias
    */
    std::map<std::string,std::string> memeMap;

    // split on & frist
    std::vector<std::string> items;
    boost::split(items, body, boost::is_any_of("&"), boost::token_compress_on);

    // bad request checks
    if (items.size() < 3) { return memeMap; }

    for (size_t i = 0; i < items.size(); i++)
    {
        std::vector<std::string> key_value;
        boost::split(key_value, items[i], boost::is_any_of("="), boost::token_compress_on);
        if (key_value.size() != 2) { return memeMap; }

        // replace "%2F" with "/" if needed
        int symbol_ind = key_value[1].find("%2F");
        if (symbol_ind != std::string::npos) { key_value[1].replace(symbol_ind, 3, "/"); }
        
        // replace "+" with " " if needed
        std::string::size_type n = 0;
        while ((n = key_value[1].find("+", n)) != std::string::npos)
        {
            key_value[1].replace(n, 1, " ");
            n += 1; // update for added " "
        }

        // add key value to memeMap
        memeMap[key_value[0]] = key_value[1];
    }
    return memeMap;
}

bool MemeHandler::MemeView()
{
    int meme_id_index = memepage_.find("id=");
    std::string meme_id;
    if (meme_id_index != std::string::npos) 
    {
        meme_id = memepage_.substr(meme_id_index + 3, memepage_.length() - 1);
    }
    else
    {
        BOOST_LOG_TRIVIAL(trace) << "Invalid meme id...";
        return true;
    }

    std::map<std::string,std::string> meme_object = MemeDB::getMemeEntriesById(meme_id);
    std::string meme_object_img = meme_object["image"];
    std::string meme_object_top = meme_object["top"];
    std::string meme_object_bot = meme_object["bottom"];

    memebody_ = "<html>"
                    "<style>"
                        "body { display: inline-block; position: relative; }"
                        "span { color: white; font: 2em bold Impact, sans-serif; position: absolute; text-align: center; width: 100%; }"
                        "#top { top: 0; left: 0; font-family: \"Impact\", Charcoal, sans-serif;}"
                        "#bottom { bottom: 0; left: 0; font-family: \"Impact\", Charcoal, sans-serif;}"
                    "</style>"
                    "<body>"
                        "<img src=\"http://localhost:8080/" + meme_object_img + "\">" // change to GCP 
                        "<span id=\"top\">" + meme_object_top + "</span>"
                        "<span id=\"bottom\">" + meme_object_bot + "</span>"
                    "</body>"
                "</html>";

    return false;
}

bool MemeHandler::MemeList()
{
    std::vector<std::map<std::string,std::string>> memelist = MemeDB::getMemeEntries();
    // build body string
    memebody_ += "<html>\n<body>";
    memebody_ += "<h2>SPICY MEME LIST</h2>\n";

    // bring in meme information and create links TODO: adjust according to how the information is stored. 

    for (int i = 0; i < memelist.size(); i++) 
    {
        memebody_ += "<a href=\"http://localhost:8080/meme/view?id=" + memelist[i]["meme-id"] +"\">";
        memebody_ += "MemeID: " + memelist[i]["meme-id"]+ " | Image: " + memelist[i]["image"] + " | Top Text: " + memelist[i]["top"] + " | Bottom Text: " + memelist[i]["bottom"]+ "\n";
        memebody_ += "</a><br />\n";
    }
    
    memebody_ += "</body>\n</html>";
    return false;
}
