#include <memory>
#include <iostream>
#include <streambuf>
#include <istream>
#include "session.h"
#include "request.h"
#include "response.h"
#include "echo_handler.h"
#include "error_handler.h"
#include "static_handler.h"
#include "status_obj.h"
#include "dispatcher.h"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/system/system_error.hpp>

void session::start()
{
    BOOST_LOG_TRIVIAL(trace) << "Session has started...";
    try { BOOST_LOG_TRIVIAL(info) << "New connection from IP: " << socket_.remote_endpoint().address().to_string(); }
    catch(boost::system::system_error const& e) { std::cout << e.what() << ": " << e.code() << " - " << e.code().message() << "\n"; }

    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{
    if (error == boost::asio::error::eof)
    {
        BOOST_LOG_TRIVIAL(trace) << "EOF received...";
        try { BOOST_LOG_TRIVIAL(info) << "Dropped connection from IP: " << socket_.remote_endpoint().address().to_string() << "..."; }
        catch(boost::system::system_error const& e) { std::cout << e.what() << ": " << e.code() << " - " << e.code().message() << "\n"; }

        return;
    }
    if (!error)
    {
        std::string httpresponse;
        
        BOOST_LOG_TRIVIAL(trace) << "Sending data to request handler...";
        std::unique_ptr<Request> req = Request::request_handler(data_);
        if (req != nullptr)
        {
            // view data members of request
            BOOST_LOG_TRIVIAL(trace) << "Info about request{ "
                                    << "Method: " << req->method()
                                    << ", URI Path: " << req->uri_path()
                                    << ", HTTP Version: " << req->http_version()
                                    << " }";

            // construct dispatcher and handle incoming request
            BOOST_LOG_TRIVIAL(info) << "Constructing dispatcher...";
            Dispatcher dispatcher(config_);
            dispatcher.dispatch(req.get());

            // add data to status database
            Response* resp = dispatcher.getResponse();
            config_->getStatusObject()
                .addStatusEntry(req->uri_path(), std::to_string(resp->getStatusCode()));
        }
        else
        {
            std::string inc_req = "Incomplete request!\r\n\r\n";
            httpresponse = inc_req.c_str();
        }

        // write response to client
        boost::asio::async_write(socket_,
            boost::asio::buffer(httpresponse.c_str(), httpresponse.length()),
            boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "async_read_some failed...";
        delete this;
    }
    
}

void session::handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
        try { BOOST_LOG_TRIVIAL(info) << "Writing response to IP: " << socket_.remote_endpoint().address().to_string() << "..."; }
        catch(boost::system::system_error const& e) { std::cout << e.what() << ": " << e.code() << " - " << e.code().message() << "\n"; }
        
        // write response to client
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