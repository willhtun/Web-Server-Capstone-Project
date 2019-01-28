#include <boost/asio.hpp>
#include "gtest/gtest.h"
#include "session.h"

using boost::asio::ip::tcp;

class SessionTest : public ::testing::Test {
  protected:
};

TEST_F(SessionTest, SocketTest) {
    // also tests session's socket function
    boost::asio::io_service io_service_;
    session s(io_service_);
    io_service_.run();
    EXPECT_EQ(1,1);
}

TEST_F(SessionTest, StartTest) {
    boost::asio::io_service io_service_;
    session s(io_service_);
    s.start();
    EXPECT_EQ(1,1);
}
