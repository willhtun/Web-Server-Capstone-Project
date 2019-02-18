#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "config_parser.h"

using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service& io_service, NginxConfig* config)
    : socket_(io_service) 
  {
    config_ = config;
  }

  tcp::socket& socket() 
  {
    return socket_;
  }

  void start();

  friend class Sesh;

private:
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);
  void handle_write(const boost::system::error_code& error);

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  NginxConfig* config_;
};


class Sesh {
  public:
    void call_handle_read(session* sesh, const boost::system::error_code& error,
      size_t bytes_transferred) {
          sesh->handle_read(error, bytes_transferred);
      }
    void call_handle_write(session* sesh, const boost::system::error_code& error) {
        sesh->handle_write(error);
    }
    void assign_data_and_call_read(session* sesh, const boost::system::error_code& error,
      size_t bytes_transferred, const char* data) {
        strncpy(sesh->data_, data, sizeof(sesh->data_)-1);
        sesh->data_[sizeof(sesh->data_)-1] = 0;
        sesh->handle_read(error, bytes_transferred);
    }
};
