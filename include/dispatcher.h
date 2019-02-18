#pragma once
#include <map>
#include "config_parser.h"
#include "request.h"
#include "response.h"

class Dispatcher {
  public:
    Dispatcher (NginxConfig* config);
    void dispatch(Request* req);
  private:
    std::string root_;
    std::map<std::string, NginxConfig*> configTable_;
    std::map<std::string, std::string> handlerTable_;
};