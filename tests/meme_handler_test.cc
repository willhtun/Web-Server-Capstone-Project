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
#include "meme_db.h"

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
};

TEST_F(MemeHandlerTest, CreatePageTest) {
    std::unique_ptr<Request> req = make_request("GET /meme/create HTTP/1.1\r\n\r\n");
    init_meme_handler("../tests/configs/echo_server_config");

    std::unique_ptr<Response> resp = handler_->HandleRequest(*(req.get()));

    EXPECT_EQ(resp->getStatusCode(), Response::OK);
}