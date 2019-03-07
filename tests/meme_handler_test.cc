#include <string>
#include <fstream>
#include <streambuf>
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "gmock/gmock.h"
#include "session.h"
#include "response.h"
#include "request.h"
#include "dispatcher.h"
#include "handler_manager.h"
#include "meme_handler.h"


class MemeHandlerTest : public ::testing::Test 
{
    protected:
        boost::asio::io_service io_service_;
        NginxConfig out_config_;
        NginxConfigParser parser_;
        RequestHandler* handler_;

    void init_meme_handler(std::string config_file) {
        parser_.Parse(config_file.c_str(), &out_config_);
        handler_ = MemeHandler::create(out_config_, "/usr/src/project");
    }

    std::unique_ptr<Request> make_request(std::string raw_req) {
        return Request::request_handler(raw_req);
    } 

    std::string get_req_string(std::string request_file_str) 
    {
        std::stringstream str;
        std::ifstream stream(request_file_str);
        if (stream.is_open()) 
        {
            while(stream.peek() != EOF)
                str << (char) stream.get();
            stream.close();
            return str.str();
        }
    }
};

//-----------Create Tests -----------//
TEST_F(MemeHandlerTest, GoodCreatePageTest) {
    //std::unique_ptr<Request> req = make_request(get_req_string("../tests/meme_handler_tests/create_request"));
    std::unique_ptr<Request> req = make_request("GET /meme/create HTTP/1.1\r\n\r\n");
    init_meme_handler("../tests/configs/meme_config");
    
    std::unique_ptr<Response> resp = handler_->HandleRequest(*(req.get()));
    EXPECT_EQ(resp->getStatusCode(), Response::OK);
}

TEST_F(MemeHandlerTest, BadCreatePageTest) {
    //std::unique_ptr<Request> req = make_request(get_req_string("../tests/meme_handler_tests/create_request"));
    std::unique_ptr<Request> req = make_request("GET /meme/create HTTP/1.1\r\n\r\n");
    init_meme_handler("../tests/configs/bad_meme_config");
    
    std::unique_ptr<Response> resp = handler_->HandleRequest(*(req.get()));
    EXPECT_EQ(resp->getStatusCode(), Response::NOT_FOUND);
}

//-----------List Tests -----------//
TEST_F(MemeHandlerTest, ListPageTest) {
    std::unique_ptr<Request> req = make_request("GET /meme/list HTTP/1.1\r\n\r\n");
    init_meme_handler("../tests/configs/meme_config");

    std::unique_ptr<Response> resp = handler_->HandleRequest(*(req.get()));
    EXPECT_EQ(resp->getStatusCode(), Response::OK);
}


//-----------View Tests -----------//
TEST_F(MemeHandlerTest, BadViewPageTest) {
    std::unique_ptr<Request> req = make_request("GET /meme/view HTTP/1.1\r\n\r\n");
    init_meme_handler("../tests/configs/meme_config");

    std::unique_ptr<Response> resp = handler_->HandleRequest(*(req.get()));

    EXPECT_EQ(resp->getStatusCode(), Response::NOT_FOUND); 
}

TEST_F(MemeHandlerTest, PostAndViewPageTest) {
    std::unique_ptr<Request> req = make_request(get_req_string("../tests/meme_handler_tests/post_request"));
    init_meme_handler("../tests/configs/meme_config");

    std::unique_ptr<Response> resp = handler_->HandleRequest(*(req.get()));

    std::unique_ptr<Request> req2 = make_request("GET /meme/viewid=25 HTTP/1.1\r\n\r\n");
    init_meme_handler("../tests/configs/meme_config");

    std::unique_ptr<Response> resp2 = handler_->HandleRequest(*(req2.get()));

    EXPECT_EQ(resp2->getStatusCode(), Response::OK); 
}

//-----------Post Tests -----------//
TEST_F(MemeHandlerTest, PostTest) {
    std::unique_ptr<Request> req = make_request(get_req_string("../tests/meme_handler_tests/post_request"));
    init_meme_handler("../tests/configs/meme_config");

    std::unique_ptr<Response> resp = handler_->HandleRequest(*(req.get()));

    EXPECT_EQ(resp->getStatusCode(), Response::OK);
}

TEST_F(MemeHandlerTest, PostDBFailTest) {
    std::unique_ptr<Request> req = make_request(get_req_string("../tests/meme_handler_tests/post_request"));
    init_meme_handler("../tests/configs/bad_meme_config");

    std::unique_ptr<Response> resp = handler_->HandleRequest(*(req.get()));

    EXPECT_EQ(resp->getStatusCode(), Response::OK); // This shouldn't work.
}

//-----------Error Tests -----------//
TEST_F(MemeHandlerTest, ErrorTest) {
    std::unique_ptr<Request> req = make_request("GET /meme/missing HTTP/1.1\r\n\r\n");
    init_meme_handler("../tests/configs/meme_config");

    std::unique_ptr<Response> resp = handler_->HandleRequest(*(req.get()));

    EXPECT_EQ(resp->getStatusCode(), Response::NOT_FOUND);
}
