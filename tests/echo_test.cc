#include "gtest/gtest.h"
#include "config_parser.h"
#include "server_object.h"
#include "server.h"

// define fixture
class EchoConfigParserTest : public ::testing::Test {
  protected:
    NginxConfigParser parser_;
    NginxConfig out_config_;

    bool parseFile(std::string config_file) {
      return parser_.Parse(config_file.c_str(), &out_config_);
    }

};

// test given example
TEST_F(EchoConfigParserTest, ExampleConfig) {
  EXPECT_TRUE(parseFile("../tests/configs/example_config"));
  // out_config should have indicated port
  out_config_.GetServerObject();
  EXPECT_EQ(ServerObject::port, 80);
}

// check big example
TEST_F(EchoConfigParserTest, FullConfig) {
  EXPECT_TRUE(parseFile("../tests/configs/full_config"));
  out_config_.GetServerObject();
  EXPECT_EQ(ServerObject::port, 80);
}

// check on separate large example for depth of validity
TEST_F(EchoConfigParserTest, FullConfig2) {
  EXPECT_TRUE(parseFile("../tests/configs/full_config2"));
  out_config_.GetServerObject();
  EXPECT_EQ(ServerObject::port, 80);
}



