#include <memory>
#include <iostream>
#include <streambuf>
#include <istream>
#include "session.h"
#include "request.h"

void session::start()
{
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

/*
    Handles weird error of CR and NL being interpreted as two characters.

    Replaces those with a singular character.
*/
std::string session::process_data()
{
    std::string new_data;
    for (size_t i = 0; i < strlen(data_); i++)
    {
        if (i != (strlen(data_)-1)) {
            if (data_[i] == '\\' && data_[i+1] == 'r')
            {
                i++;
                new_data += '\r';
            }  
            else if (data_[i] == '\\' && data_[i+1] == 'n')
            {
                i++;
                new_data += '\n';
            }
            else{
                new_data += data_[i];
            }
        }
    }
    return new_data;
}

// Ex GET request? GET / HTTP/1.1\r\nHost: www.example.com\r\nConnection: close\r\n\r\n

void session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{
    if (!error)
    {
      // print original data
      std::cout << "My input: " <<  data_ << std::endl;
      
      // process data
      //std::string data = session::process_data();
      
      // send data
      bool COMPLETE_ERROR = true;
      
      std::unique_ptr<Request> req = Request::request_handler(data_);
      if (req != nullptr)
      {
          // view data members
          std::cout << req->method() << std::endl;
          std::cout << req->uri_path() << std::endl;
          std::cout << req->http_version() << std::endl;
          COMPLETE_ERROR = false;
      }

      //Writes back the response code and content type to the client
      const char* httpresponse;
      if (!COMPLETE_ERROR)
      {
          httpresponse = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"; //The default response if request is complete
      }
      else
      {
          httpresponse = "Incomplete request!\r\n\r\n";
      }
      boost::asio::async_write(socket_,
          boost::asio::buffer(httpresponse, strlen(httpresponse)),
          boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));

      //Writes back the request in the body of the response
      boost::asio::async_write(socket_,
          boost::asio::buffer(data_, bytes_transferred),
          boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));

      
    }
    else
    {
      delete this;
    }
}

void session::handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      delete this;
    }
}