#include <string>
#include <fstream>
#include <streambuf>
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "session.h"
#include "response.h"

class StaticHandlerTest : public ::testing::Test {
  protected:
    boost::asio::io_service io_service_;

    void read_request_file(std::string request_file_str) {
        std::cout << "HEHEHEHEHEH" << std::endl;
        const char* a_request = file_to_string(request_file_str);
        session* s = new session(io_service_);
        Sesh sesh;
        boost::system::error_code ec;
        sesh.assign_data_and_call_read(s, ec, 100, a_request);
    }

    const char* file_to_string(std::string request_file_str) {
        std::stringstream str;
        std::ifstream stream(request_file_str);
        if (stream.is_open()) {
            while(stream.peek() != EOF) {
                str << (char) stream.get();
            }
            stream.close();
            std::cout << str.str() << std::endl;
            std::cout << str.str().c_str() << std::endl;
            return str.str().c_str();
        }
    }
};


TEST_F(StaticHandlerTest, RegularStaticCall) {
    read_request_file("../tests/static_handler_tests/photo_request");
    EXPECT_EQ(1,1);
}