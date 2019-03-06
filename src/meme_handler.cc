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
        std::string redirect_id;
        // add meme-id with correct locks in place to database
        mtx.lock();
        //std::string meme_id = std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())); // still does not solve race condition completely
        redirect_id = AddToDatabase(memeMap["name"], memeMap["image"], memeMap["top"], memeMap["bottom"]);
        mtx.unlock();

        errorflag = MemeResult(redirect_id);                    
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
    // URL to ASCII decoding and
    // split on & frist
    std::vector<std::string> items;
    boost::split(items, body, boost::is_any_of("&"), boost::token_compress_on);
    bool deepfry = false;

    // bad request checks
    if (items.size() < 3) { return memeMap; }

    for (size_t i = 0; i < items.size(); i++)
    {

        std::vector<std::string> key_value;
        boost::split(key_value, items[i], boost::is_any_of("="), boost::token_compress_on);
        key_value[1] = URLParser::urlDecode(key_value[1]);

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
        if (key_value[0] == "deepfry" && key_value[1] == "True")
            deepfry = true;

        memeMap[key_value[0]] = key_value[1];
    }

    if (deepfry)
    {
        memeMap["image"] = "df-" + memeMap["image"];
    }
    memeMap["image"] = "meme_templates/" + memeMap["image"];

    memeMap["name"] = URLParser::htmlEncode(memeMap["name"]);
    memeMap["top"] = URLParser::htmlEncode(memeMap["top"]);
    memeMap["bottom"] = URLParser::htmlEncode(memeMap["bottom"]);

    return memeMap;
}

bool MemeHandler::MemeView()
{
    /*
        Returns an html page pouplated with the meme object associated with the meme ID

        Author: Will Htun
    */
    std::string meme_object_name;
    std::string meme_object_img;
    std::string meme_object_top;
    std::string meme_object_bot;

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

    // SQL SELECT operation
    int rc;
    sqlite3_stmt *stmt;
    rc = sqlite3_open(("../" + savedir_ + "/meme_vault.db").c_str(), &db);
    if(rc) 
    {
        BOOST_LOG_TRIVIAL(trace) << "Error opening database...";
        sqlite3_close(db);
        return false;
    } 
    BOOST_LOG_TRIVIAL(trace) << "Opened database for writing...";
    // Injection Test
    // meme_id = "1551771020; DROP TABLE MEME_HISTORY";

    std::string selector = "SELECT * FROM MEME_HISTORY WHERE MEME_ID = ?";
    rc = sqlite3_prepare_v2(db, selector.c_str(), selector.length(), &stmt, NULL);
    if( rc == SQLITE_OK ) {
        sqlite3_bind_text(stmt, 1, meme_id.c_str(), meme_id.length(), 0);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            meme_object_name = strdup((const char*)sqlite3_column_text(stmt, 1));
            meme_object_img = strdup((const char*)sqlite3_column_text(stmt, 2));
            meme_object_top = strdup((const char*)sqlite3_column_text(stmt, 3));
            meme_object_bot = strdup((const char*)sqlite3_column_text(stmt, 4));
        }
        sqlite3_finalize(stmt);
    }
    //rc = sqlite3_exec(db, selector.c_str(), callback_GetEntryFromDatabase, &meme_object, NULL);
    if(rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(trace) << "Error reading table...";
    } 

    sqlite3_close(db);
    BOOST_LOG_TRIVIAL(trace) << "Closed database";





    std::ifstream ifs(".." + filedir_ + "/view.html", std::ios::in | std::ios::binary);
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
    boost::replace_all(memebody_, "meme_object_name", meme_object_name);
    boost::replace_all(memebody_, "meme_object_img", meme_object_img);
    boost::replace_all(memebody_, "meme_object_top", meme_object_top);
    boost::replace_all(memebody_, "meme_object_bot", meme_object_bot);

    return false;
}

bool MemeHandler::MemeResult(std::string id_)
{
    /*
        Returns an html page pouplated with the meme object associated with the meme ID

        Author: Will Htun
    */
   
    std::string meme_object_name;
    std::string meme_object_img;
    std::string meme_object_top;
    std::string meme_object_bot;

    // SQL SELECT operation
    int rc;
    sqlite3_stmt *stmt;
    rc = sqlite3_open(("../" + savedir_ + "/meme_vault.db").c_str(), &db);
    if(rc) 
    {
        BOOST_LOG_TRIVIAL(trace) << "Error opening database...";
        sqlite3_close(db);
        return false;
    } 
    BOOST_LOG_TRIVIAL(trace) << "Opened database for writing...";

    std::string selector = "SELECT * FROM MEME_HISTORY WHERE MEME_ID = ?";
    rc = sqlite3_prepare_v2(db, selector.c_str(), selector.length(), &stmt, NULL);
    if(rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, id_.c_str(), id_.length(), 0);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            meme_object_name = strdup((const char*)sqlite3_column_text(stmt, 1));
            meme_object_img = strdup((const char*)sqlite3_column_text(stmt, 2));
            meme_object_top = strdup((const char*)sqlite3_column_text(stmt, 3));
            meme_object_bot = strdup((const char*)sqlite3_column_text(stmt, 4));
        }
        sqlite3_finalize(stmt);
    }

   //rc = sqlite3_exec(db, selector.c_str(), callback_GetEntryFromDatabase, &meme_object, NULL);
    if(rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(trace) << "Error reading table...";
    } 

    sqlite3_close(db);
    BOOST_LOG_TRIVIAL(trace) << "Closed database";

    std::ifstream ifs(".." + filedir_ + "/view.html", std::ios::in | std::ios::binary);
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

    boost::replace_all(memebody_, "meme_object_name", meme_object_name);
    boost::replace_all(memebody_, "meme_object_img", meme_object_img);
    boost::replace_all(memebody_, "meme_object_top", meme_object_top);
    boost::replace_all(memebody_, "meme_object_bot", meme_object_bot);

    return false;
}

bool MemeHandler::MemeList()
{
    std::vector<std::map<std::string,std::string>> memelist = GetAllFromDatabase();
    // build body string
    std::ifstream ifs(".." + filedir_ + "/list.html", std::ios::in | std::ios::binary);
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

    // bring in meme information and create links TODO: adjust according to how the information is stored. 

    for (int i = 0; i < memelist.size(); i++) 
    {
        memebody_ += "<a id=\"entry\" href=\"http://localhost:8082/meme/view?id=" + memelist[i]["MEME_ID"] +"\">";
     //   memebody_ += "MemeID: " + memelist[i]["MEME_ID"]+ " | Name: " + memelist[i]["NAME"] + " | Image: " + memelist[i]["IMAGE"] + " | Top Text: " + memelist[i]["TOP"] + " | Bottom Text: " + memelist[i]["BOTTOM"]+ "\n";
        memebody_ += "MemeID: " + memelist[i]["MEME_ID"]+ " | Name: " + memelist[i]["NAME"] + "\n";
        memebody_ += "</a><br />\n";
    }
    
    memebody_ += "</div>\n</div>\n";
    memebody_ += "</body>\n</html>";
    return false;
}

std::string MemeHandler::AddToDatabase(std::string name_, std::string image_, std::string top_, std::string bottom_) {
    /*
        Add an entry to the database with the given values.

        Author: Will Htun
    */

    // SQL CREATE operation
    int rc;
    std::string id_ = "10000";
    sqlite3_stmt *stmt;
    rc = sqlite3_open(("../" + savedir_ + "/meme_vault.db").c_str(), &db);
    if(rc) 
    {
        BOOST_LOG_TRIVIAL(trace) << "Error opening/creating database...";
        sqlite3_close(db);
        errorflag = true;
    } 
    BOOST_LOG_TRIVIAL(trace) << "Opened database for writing...";

    std::string table = "CREATE TABLE IF NOT EXISTS MEME_HISTORY("
      "MEME_ID  TEXT     PRIMARY KEY  NOT NULL,"
      "NAME     TEXT                  NOT NULL,"
      "IMAGE    TEXT                  NOT NULL,"
      "TOP      TEXT                  NOT NULL,"
      "BOTTOM   TEXT                  NOT NULL);";

    rc = sqlite3_exec(db, table.c_str(), NULL, NULL, NULL);
    if(rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(trace) << "Error creating table...";
        sqlite3_close(db);
        errorflag = true;
    } 
    BOOST_LOG_TRIVIAL(trace) << "Successfully created table!";
    // SQL SELECT operation for counter
    std::string selector = ("SELECT * FROM MEME_HISTORY WHERE MEME_ID = (SELECT MAX(MEME_ID) FROM MEME_HISTORY)");
    rc = sqlite3_prepare_v2(db, selector.c_str(), selector.length(), &stmt, NULL);
    if(rc == SQLITE_OK) 
    {
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            id_ = strdup((const char*)sqlite3_column_text(stmt, 0));
            id_ = std::to_string(stoi(id_) + 1);
        }
        sqlite3_finalize(stmt);
    }
    // SQL INSERT operation
    std::string entry = "INSERT INTO MEME_HISTORY (MEME_ID,NAME,IMAGE,TOP,BOTTOM) "
                         "VALUES (?,?,?,?,?);";

    rc = sqlite3_prepare_v2(db, entry.c_str(), entry.length(), &stmt, NULL);
    if( rc == SQLITE_OK ) {
        // bind the value 
        sqlite3_bind_text(stmt, 1, id_.c_str(), id_.length(), 0);
        sqlite3_bind_text(stmt, 2, name_.c_str(), name_.length(), 0);
        sqlite3_bind_text(stmt, 3, image_.c_str(), image_.length(), 0);
        sqlite3_bind_text(stmt, 4, top_.c_str(), top_.length(), 0);
        sqlite3_bind_text(stmt, 5, bottom_.c_str(), bottom_.length(), 0);

        // commit 
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    //rc = sqlite3_exec(db, entry.c_str(), NULL, NULL, NULL);
    if(rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(trace) << "Error adding entry to table...";
        sqlite3_close(db);
        errorflag = true;
    } 
    BOOST_LOG_TRIVIAL(trace) << "Successfully added entry!";

    sqlite3_close(db);
    BOOST_LOG_TRIVIAL(trace) << "Closed database";
    errorflag = false;
    return id_;
}

std::vector<std::map<std::string,std::string>> MemeHandler::GetAllFromDatabase() {
    /*
        Returns all the memes in the database in the form of a vector of maps.

        Author: Will Htun
    */

    std::vector<std::map<std::string,std::string>> meme_entries_;

    // SQL SELECT operation
    int rc;
    rc = sqlite3_open(("../" + savedir_ + "/meme_vault.db").c_str(), &db);

    if(rc) 
    {
        BOOST_LOG_TRIVIAL(trace) << "Error opening/creating database...";
    } 
    BOOST_LOG_TRIVIAL(trace) << "Opened database for reading...";

    std::string selector = "SELECT * FROM MEME_HISTORY";
    rc = sqlite3_exec(db, selector.c_str(), callback_GetAllFromDatabase, &meme_entries_, NULL);
    if(rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(trace) << "Error reading table...";
    } 

    sqlite3_close(db);
    BOOST_LOG_TRIVIAL(trace) << "Closed database";
    return meme_entries_;
}