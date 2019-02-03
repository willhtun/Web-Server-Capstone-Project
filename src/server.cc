#include "server.h"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

void server::start_accept()
{
    session* new_session = new session(io_service_);
    BOOST_LOG_TRIVIAL(trace) << "New session started...";
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session,
    const boost::system::error_code& error)
{
    if (!error)
    {
        BOOST_LOG_TRIVIAL(trace) << "Starting new session...";
        new_session->start();
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Error! Session did not start...";
        delete new_session;
    }

    start_accept();
}