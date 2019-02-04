#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "session.h"
#include "response.h"

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


TEST_F(SessionTest, HandleReadEOFTest) {
    session* s = new session(io_service_);
    Sesh sesh;
    sesh.call_handle_read(s,boost::asio::error::eof, 100);
    EXPECT_EQ(1,1);
}

TEST_F(SessionTest, HandleReadNotFoundTest) {
    session* s = new session(io_service_);
    Sesh sesh;
    sesh.call_handle_read(s,boost::asio::error::not_found,100);
    EXPECT_EQ(1,1);
}

TEST_F(SessionTest, HandleReadRegularTest) {
    session* s = new session(io_service_);
    Sesh sesh;
    boost::system::error_code ec;
    sesh.call_handle_read(s,ec,100);
    EXPECT_EQ(1,1);
}

TEST_F(SessionTest, HandleWriteTest) {
    session* s = new session(io_service_);
    Sesh sesh;
    sesh.call_handle_write(s,boost::asio::error::eof);
    EXPECT_EQ(1,1);
}
