#include <string>
#include <vector>
#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include "meme_handler.h"
#include <sstream>
#include <sstream> 
#include <fstream>
#include <thread>
#include <mutex>
#include <string>

std::mutex mtx;

RequestHandler* MemeHandler::create(const NginxConfig& config, const std::string& path)
{
    MemeHandler* mh = new MemeHandler();
    mh->root_ = path;
    mh->filedir_ = config.GetAttribute("location");
    mh->savedir_ = config.GetAttribute("save");
    mh->uri_ = config.GetAttribute("url");

    BOOST_LOG_TRIVIAL(trace) << "Meme handler created";

    return mh;
};

std::unique_ptr<Response> MemeHandler::HandleRequest(const Request& request)
{
    BOOST_LOG_TRIVIAL(trace) << "Meme handler building response for request...";

    std::string full_url = request.uri_path();
    memepage_ = full_url.substr(uri_.length() + 1, full_url.length());

    if (memepage_ == "create") //Landing Page for Meme Creation
    {
        errorflag = MemeCreate();
    }
    else if(memepage_ == "list") // List all created memes
    {
        errorflag = MemeList();
    }
    else if(memepage_.substr(0,4) == "view") // Page to view a meme
    {
        errorflag = MemeView();
    }
    else if (request.method() == "POST") {
        std::map<std::string,std::string> memeMap = parseRequestBody(request.body());

        // add meme-id with correct locks in place to database
        mtx.lock();
        std::string id_ = std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())); // still does not solve race condition completely
        AddToDatabase(id_, memeMap["image"], memeMap["top"], memeMap["bottom"]);
        mtx.unlock();

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
    return response;
};

static int callback_GetEntryFromDatabase(void* ptr, int argc, char **argv, char **azColName){
    /*
        Callback function for SQL command. ptr should be a pointer to map to be populated with the search result.

        Author: Will Htun
    */
    std::map<std::string,std::string>* entry = static_cast<std::map<std::string,std::string>*>(ptr);
    for(int i = 0; i<argc; i++)
    {
        entry->insert(std::pair<std::string,std::string>(std::string(azColName[i]),std::string(argv[i])));
    }
    return 0;
}

static int callback_GetAllFromDatabase(void* ptr, int argc, char **argv, char **azColName){
    /*
        Callback function for SQL command. Returns a vector of maps to the address pointer 
        by ptr. ptr must point to a vector map.

        Author: Will Htun
    */

    std::vector<std::map<std::string,std::string>>* vectormap = static_cast<std::vector<std::map<std::string,std::string>>*>(ptr);
    std::map<std::string,std::string>* entry = new std::map<std::string,std::string>();
    for(int i = 0; i<argc; i++)
    {
        entry->insert(std::pair<std::string,std::string>(std::string(azColName[i]),std::string(argv[i])));
    }
    vectormap->push_back(*entry);
    return 0;
}

bool MemeHandler::MemeCreate()
{
    /*
        Hosts create.html page. Returns false if we achieved no error.
    */
    std::ifstream ifs(".." + filedir_ + "/create.html", std::ios::in | std::ios::binary);
    if (!ifs.is_open() || memepage_.length() == 0)
    {
        return true;
    }

    char buf[512];
    while (ifs.read(buf, sizeof(buf)).gcount() > 0) 
    {
        memebody_.append(buf, ifs.gcount());
    }
    
    ifs.close();
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
    /*
        Returns an html page pouplated with the meme object associated with the meme ID

        Author: Will Htun
    */

    // Extract meme ID
    int meme_id_index = memepage_.find("id=");
    std::map<std::string,std::string> meme_object;
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

    // SQL SELECT operation
    int rc;
    rc = sqlite3_open(("../" + savedir_ + "/test.db").c_str(), &db);
    if(rc) 
    {
        BOOST_LOG_TRIVIAL(trace) << "Error opening database...";
        sqlite3_close(db);
        return false;
    } 
    BOOST_LOG_TRIVIAL(trace) << "Opened database for writing...";

    std::string selector = "SELECT * FROM MEME_HISTORY WHERE MEME_ID=" + meme_id;
    rc = sqlite3_exec(db, selector.c_str(), callback_GetEntryFromDatabase, &meme_object, NULL);
    if(rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(trace) << "Error reading table...";
    } 

    sqlite3_close(db);
    BOOST_LOG_TRIVIAL(trace) << "Closed database";

    // Populate HTML
    std::string meme_object_img = meme_object["IMAGE"];
    std::string meme_object_top = meme_object["TOP"];
    std::string meme_object_bot = meme_object["BOTTOM"];

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
    std::vector<std::map<std::string,std::string>> memelist = GetAllFromDatabase();
    // build body string
    memebody_ += "<html>\n<body>";
    memebody_ += "<h2>SPICY MEME LIST</h2>\n";

    // bring in meme information and create links TODO: adjust according to how the information is stored. 

    for (int i = 0; i < memelist.size(); i++) 
    {
        memebody_ += "<a href=\"http://localhost:8080/meme/view?id=" + memelist[i]["MEME_ID"] +"\">";
        memebody_ += "MemeID: " + memelist[i]["MEME_ID"]+ " | Image: " + memelist[i]["IMAGE"] + " | Top Text: " + memelist[i]["TOP"] + " | Bottom Text: " + memelist[i]["BOTTOM"]+ "\n";
        memebody_ += "</a><br />\n";
    }
    
    memebody_ += "</body>\n</html>";
    return false;
}

bool MemeHandler::AddToDatabase(std::string id_, std::string image_, std::string top_, std::string bottom_) {
    /*
        Add an entry to the database with the given values.

        Author: Will Htun
    */

    // SQL CREATE operation
    int rc;
    rc = sqlite3_open(("../" + savedir_ + "/test.db").c_str(), &db);
    if(rc) 
    {
        BOOST_LOG_TRIVIAL(trace) << "Error opening/creating database...";
        sqlite3_close(db);
        return false;
    } 
    BOOST_LOG_TRIVIAL(trace) << "Opened database for writing...";

    std::string table = "CREATE TABLE IF NOT EXISTS MEME_HISTORY("
      "MEME_ID  TEXT     PRIMARY KEY  NOT NULL,"
      "IMAGE    TEXT                  NOT NULL,"
      "TOP      TEXT                  NOT NULL,"
      "BOTTOM   TEXT                  NOT NULL);";

    rc = sqlite3_exec(db, table.c_str(), NULL, NULL, NULL);
    if(rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(trace) << "Error creating table...";
        sqlite3_close(db);
        return false;
    } 
    BOOST_LOG_TRIVIAL(trace) << "Successfully created table!";

    // SQL INSERT operation
    std::string entry = "INSERT INTO MEME_HISTORY (MEME_ID,IMAGE,TOP,BOTTOM) "
                        "VALUES (" + id_ + ", '" + image_ + "', '" + top_ + "', '" + bottom_ + "');";

    rc = sqlite3_exec(db, entry.c_str(), NULL, NULL, NULL);
    if(rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(trace) << "Error adding entry to table...";
        sqlite3_close(db);
        return false;
    } 
    BOOST_LOG_TRIVIAL(trace) << "Successfully added entry!";

    sqlite3_close(db);
    BOOST_LOG_TRIVIAL(trace) << "Closed database";
    return true;
}

std::vector<std::map<std::string,std::string>> MemeHandler::GetAllFromDatabase() {
    /*
        Returns all the memes in the database in the form of a vector of maps.

        Author: Will Htun
    */

    std::vector<std::map<std::string,std::string>> meme_entries_;

    // SQL SELECT operation
    int rc;
    rc = sqlite3_open(("../" + savedir_ + "/test.db").c_str(), &db);

    if(rc) 
    {
        BOOST_LOG_TRIVIAL(trace) << "Error opening/creating database...";
    } 
    BOOST_LOG_TRIVIAL(trace) << "Opened database for reading...";

    std::string selector = "SELECT * from MEME_HISTORY";
    rc = sqlite3_exec(db, selector.c_str(), callback_GetAllFromDatabase, &meme_entries_, NULL);
    if(rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(trace) << "Error reading table...";
    } 

    sqlite3_close(db);
    BOOST_LOG_TRIVIAL(trace) << "Closed database";
    return meme_entries_;
}