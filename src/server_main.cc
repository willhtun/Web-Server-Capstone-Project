//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp> // keyword
#include <boost/log/sinks/text_file_backend.hpp> // file_name, rotation_size, format
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/expressions/keyword_fwd.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/support/date_time.hpp>
#include "config_parser.h"
#include "server.h"
#include "response.h"

using boost::asio::ip::tcp;

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

BOOST_LOG_ATTRIBUTE_KEYWORD(thread_id, "ThreadID", boost::log::attributes::current_thread_id::value_type)

void init()
{
    logging::add_file_log
    (
        keywords::file_name = "../logs/server_%Y-%m-%d_%H-%M-%S.%N.log",                // create server log
        keywords::rotation_size = 10 * 1024 * 1024,                                     // rotate after 10Mb
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),   // new log after midnight
        keywords::format = 
        (
            expr::stream
              << expr::format_date_time< boost::posix_time::ptime > ("TimeStamp", "%Y-%m-%d %H:%M:%S") << "\t| "
              << thread_id << "\t| "
              << logging::trivial::severity << "\t| "
              << expr::message
        ),
        keywords::auto_flush = true
    );

    logging::core::get()->set_filter
    (
        // will only log items with severity level greater than trace
        logging::trivial::severity >= logging::trivial::trace
    );
}

int main(int argc, char* argv[])
{
  try
  {
    init();
    logging::add_common_attributes(); // add attributes most likely to be used

    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      BOOST_LOG_TRIVIAL(fatal) << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    BOOST_LOG_TRIVIAL(trace) << "Parsing config file...";
    NginxConfigParser config_parser; // Import necessary classes to parse config file
    NginxConfig config;
    if (!config_parser.Parse(argv[1], &config)) {
      BOOST_LOG_TRIVIAL(fatal) << "Bad config file...";
      std::cerr << "Bad config file\n";
      return 1;
    }

    boost::asio::io_service io_service;
    using namespace std; // For atoi.

    Server_o* server_config = config.GetServerObject();
    //config.ToString(); // This will popualate the Server_o struct
    if (!server_config) { // Exit with error if server block cannot be read
      BOOST_LOG_TRIVIAL(fatal) << "Bad config...";
      std::cerr << "Bad config\n";
      return 1;
    }

    if (server_config->port == 0) { // Exit with error if port is unspecified
      BOOST_LOG_TRIVIAL(fatal) << "No port specified...";
      std::cerr << "No port specified\n";
      return 1;
    } 
    if (server_config->static_directory == "") { // Do we want to exit or use root folder if location is not specified
      std::cerr << "No static directory specified\n";
      return 1;
    } 

    std::cout << server_config->port << " " << server_config->static_directory << std::endl;

    BOOST_LOG_TRIVIAL(info) << "Starting server...";
    server s(io_service, server_config->port);
    io_service.run();
  }
  catch (std::exception& e)
  {
    BOOST_LOG_TRIVIAL(fatal) << "Exception: " << e.what();
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
