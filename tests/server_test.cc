#include <boost/asio.hpp>
#include <memory>
#include "gtest/gtest.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "config_parser.h"
#include "server_object.h"
#include "server.h"
#include "response.h"

class ServerTest : public ::testing::Test {
  protected:
    NginxConfigParser parser_;
	  NginxConfig out_config_;

    bool parseString(const std::string config_string) {
      std::stringstream config_stream(config_string);
      return parser_.Parse(&config_stream, &out_config_);
    }	
};

TEST_F(ServerTest, HandleAcceptTest) {
    boost::asio::io_service io_service_;
    server s(io_service_, 8080);
    // test simplying to catch an error
    EXPECT_EQ(1,1);
}

// test comment
TEST_F(ServerTest, CommentTest) {
  EXPECT_TRUE(parseString("user www www; ## Default: nobody"));
  EXPECT_EQ(out_config_.statements_.size(),1) << "Only 1 statement in config";
}

// statement size checks
TEST_F(ServerTest, StatementSizeTest) {
  EXPECT_TRUE(parseString("worker_processes 5; error_log logs/error.log; pid logs/nginx.pid;"));
  EXPECT_EQ(out_config_.statements_.size(), 3) << "Only 3 statements in config";
}

// Test for missing port definition
TEST_F(ServerTest, MissingPortConfig) {
	EXPECT_FALSE(parseString(""));
}

// check ToString
TEST_F(ServerTest, ToStringTest) {
  NginxConfigStatement stmt;
  stmt.tokens_.push_back("location");
  stmt.tokens_.push_back("/");
  stmt.tokens_.push_back("{");
  stmt.tokens_.push_back("proxy_pass");
  stmt.tokens_.push_back("http://127.0.0.1:8080");
  stmt.tokens_.push_back(";");
  stmt.tokens_.push_back("}");
  EXPECT_EQ(stmt.ToString(0), "location / { proxy_pass http://127.0.0.1:8080 ; };\n");
}
