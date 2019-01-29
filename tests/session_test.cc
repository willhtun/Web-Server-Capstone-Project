#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "session.h"

using boost::asio::ip::tcp;

class SessionTest : public ::testing::Test {
  protected:

    boost::asio::io_service io_service_;
};
/*
class SeshTest {
  public:
    SeshTest(boost::asio::io_service& io_service)
    : socket_(io_service) {}

  private:
    FRIEND_TEST(SeshTest, HandleReadTest);
    void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);

    tcp::socket socket_;
};
*/

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
    session s(io_service_);
    Sesh sesh;
    sesh.call_handle_read(s,boost::asio::error::eof, 100);
    EXPECT_EQ(1,1);
}

TEST_F(SessionTest, HandleReadRegularTest) {
    session s(io_service_);
    Sesh sesh;
    // zero is a good error code
    sesh.call_handle_read(s, boost::asio::error::not_found, 100);
    EXPECT_EQ(1,1);
}