#include "gtest/gtest.h"
#include "config_parser.h"

// define fixture
class NginxConfigParserTest : public ::testing::Test {
  protected:
    NginxConfigParser parser_;
    NginxConfig out_config_;

    bool parseString(std::string config_string) {
      std::stringstream config_stream(config_string);
      return parser_.Parse(&config_stream, &out_config_);
    }

    bool parseFile(std::string config_file) {
      return parser_.Parse(config_file.c_str(), &out_config_);
    }

};

// test given example
TEST_F(NginxConfigParserTest, ExampleConfig) {
  EXPECT_TRUE(parseFile("example_config"));
}

// test invalid input
TEST_F(NginxConfigParserTest, InvalidInput) {
  // wrong braces
  EXPECT_FALSE(parseString("events { worker_connections 4096;"));
  EXPECT_FALSE(parseString("events worker_connections 4096; }"));
  // wrong quotes
  EXPECT_FALSE(parseString("log_format main \'$remote;"));
  EXPECT_FALSE(parseString("log_format main \"$remote;"));
  // wrong semicolon
  EXPECT_FALSE(parseString("include conf/mime.types"));
  EXPECT_FALSE(parseString("server { listen 80; };"));
  EXPECT_FALSE(parseString("server { listen 80 };"));
}

// test comment
TEST_F(NginxConfigParserTest, CommentTest) {
  EXPECT_TRUE(parseString("user www www; ## Default: nobody"));
  EXPECT_EQ(out_config_.statements_.size(),1) << "Only 1 statement in config";
}

// statement size checks
TEST_F(NginxConfigParserTest, StatementSizeTest) {
  EXPECT_TRUE(parseString("worker_processes 5; error_log logs/error.log; pid logs/nginx.pid;"));
  EXPECT_EQ(out_config_.statements_.size(), 3) << "Only 3 statements in config";
}

// check correct parsing for nested statement
TEST_F(NginxConfigParserTest, NestedParseCheck) {
  std::string config_string = 
    "events {\n "
    "worker_connections 4096; # default 1024\n "
    "worker_rlimit_profile 8192;\n"
    "}\n";
  EXPECT_TRUE(parseString(config_string));
  EXPECT_EQ(out_config_.statements_.at(0)->child_block_->statements_.size(), 2);
  EXPECT_EQ(out_config_.statements_.at(0)->child_block_->statements_.at(0)->tokens_.at(0), "worker_connections");
  EXPECT_EQ(out_config_.statements_.at(0)->child_block_->statements_.at(0)->tokens_.at(1), "4096");
  EXPECT_EQ(out_config_.statements_.at(0)->child_block_->statements_.at(1)->tokens_.at(0), "worker_rlimit_profile");
  EXPECT_EQ(out_config_.statements_.at(0)->child_block_->statements_.at(1)->tokens_.at(1), "8192");
}

// check ToString
TEST_F(NginxConfigParserTest, ToStringTest) {
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

// check big example
TEST_F(NginxConfigParserTest, FullConfig) {
  EXPECT_TRUE(parseFile("full_config"));
}

// check on separate large example for depth of validity
TEST_F(NginxConfigParserTest, FullConfig2) {
  EXPECT_TRUE(parseFile("full_config2"));
}
