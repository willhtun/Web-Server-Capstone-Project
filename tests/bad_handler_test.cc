#include "bad_handler.h"
#include <string>
#include <streambuf>
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "gmock/gmock.h"
#include "response.h"
#include "request.h"

class BadHandlerTest : public ::testing::Test 
{
protected:
    NginxConfig out_config_;
    NginxConfigParser parser_;
    RequestHandler* handler_;

    void init_bad_handler(std::string config_file) {
        parser_.Parse(config_file.c_str(), &out_config_);
        handler_ = BadHandler::create(out_config_, "/usr/src/project");
    }
    std::unique_ptr<Request> make_request(std::string raw_req) {
        return Request::request_handler(raw_req);
    }
    };


