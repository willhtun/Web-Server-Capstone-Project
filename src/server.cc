#include "server.h"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <thread>

void server::run()
{
  // Create a pool of threads to run all of the io_services.
  std::cout << "RUN\n";
  std::vector<boost::shared_ptr<boost::thread> > threads;
  for (std::size_t i = 0; i < thread_pool_size_; ++i)
  {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
          boost::bind(&boost::asio::io_service::run, &io_service_)));
    threads.push_back(thread);
  }

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
}

void server::start_accept()
{
  /*
    session* new_session = new session(io_service_, config_);
    BOOST_LOG_TRIVIAL(trace) << "Server accepting connections...";
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
          */

    while (true) {
      session* new_session = new session(io_service_, config_);
      boost::system::error_code err;
      BOOST_LOG_TRIVIAL(trace) << "Server accepting connections...";
      acceptor_.accept(new_session->socket());
      std::thread session_thread(&server::handle_accept, this, new_session, err);
      session_thread.detach();
        // boost::bind(&server::handle_accept, this, new_session,
          // boost::asio::placeholders::error);
    }
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

   // start_accept();
}

void server::handle_stop()
{
  io_service_.stop();
}