#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>

#include "session.h"

using boost::asio::ip::tcp;

class server
{
public:
  server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  {
    start_accept();
  }

  friend class Serv;

private:
  void start_accept();
  void handle_accept(session* new_session,
      const boost::system::error_code& error);
  
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};


class Serv {
  public:
    void call_start_accept(server* serv) {
      serv->start_accept();
    }
    void call_handle_accept(server* serv, session* new_session, const boost::system::error_code& error) {
      serv->handle_accept(new_session, error);
    }
};