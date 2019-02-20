#include <string>
#include <fstream>
#include <streambuf>
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "session.h"
#include "response.h"
#include "dispatcher.h"
#include "status_obj.h"
#include "handler_manager.h"

class StaticHandlerTest : public ::testing::Test {
  protected:
    boost::asio::io_service io_service_;
    NginxConfig out_config_;

    void read_request_file(std::string request_file_str) {
        const char* a_request = file_to_string(request_file_str);
        session* s = new session(io_service_, &out_config_);
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
            return str.str().c_str();
        }
    }
};


TEST_F(StaticHandlerTest, JpgRequest) {
    read_request_file("../tests/static_handler_tests/jpg_request");
    EXPECT_EQ(1,1);
}

TEST_F(StaticHandlerTest, JpegRequest) {
    read_request_file("../tests/static_handler_tests/jpeg_request");
    EXPECT_EQ(1,1);
}

TEST_F(StaticHandlerTest, PngRequest) {
    read_request_file("../tests/static_handler_tests/png_request");
    EXPECT_EQ(1,1);
}

TEST_F(StaticHandlerTest, PdfRequest) {
    read_request_file("../tests/static_handler_tests/pdf_request");
    EXPECT_EQ(1,1);
}

TEST_F(StaticHandlerTest, TiffRequest) {
    read_request_file("../tests/static_handler_tests/tiff_request");
    EXPECT_EQ(1,1);
}

TEST_F(StaticHandlerTest, DocRequest) {
    read_request_file("../tests/static_handler_tests/doc_request");
    EXPECT_EQ(1,1);
}

TEST_F(StaticHandlerTest, TxtRequest) {
    read_request_file("../tests/static_handler_tests/txt_request");
    EXPECT_EQ(1,1);
}

TEST_F(StaticHandlerTest, CsvRequest) {
    read_request_file("../tests/static_handler_tests/csv_request");
    EXPECT_EQ(1,1);
}