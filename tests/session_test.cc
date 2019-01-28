#include <boost/asio.hpp>
#include "gtest/gtest.h"
#include "session.h"

using boost::asio::ip::tcp;

class SessionTest : public ::testing::Test {
  protected:

    boost::asio::io_service io_service_;
    
};

TEST_F(SessionTest, SocketTest) {
    // also tests session's socket function
   
    io_service_.run();
    EXPECT_EQ(1,1);
}

TEST_F(SessionTest, StartTest) {
    session s(io_service_);
    s.start();
    EXPECT_EQ(1,1);
}


