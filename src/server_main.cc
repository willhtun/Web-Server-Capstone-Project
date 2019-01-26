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

#include "config_parser.h"
#include "server.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    NginxConfigParser config_parser; // Import necessary classes to parse config file
    NginxConfig config;
    if (!config_parser.Parse(argv[1], &config)) {
      std::cerr << "Bad config file\n";
      return 1;
    }

    boost::asio::io_service io_service;
    using namespace std; // For atoi.

    Server_o* server_config = config.GetServerObject();
    //config.ToString(); // This will popualate the Server_o struct
    if (!server_config) { // Exit with error if server block cannot be read
      std::cerr << "Bad config\n";
      return 1;
    }
    if (server_config->port == 0) { // Exit with error if port is unspecified
      std::cerr << "No port specified\n";
      return 1;
    } 

    server s(io_service, server_config->port);
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
