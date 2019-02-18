#include "dispatcher.h" 
#include "handle_manager.h"
#include "request_handler.h"

Dispatcher::Dispatcher (NginxConfig* config) {
  root_ = config->GetRoot();
  config->ParseString(0, configTable_, handlerTable_);
}

void Dispatcher::dispatch(Request* req) {
  HandlerManager handlermanager_;

  std::string full_url = req->uri_path();
  int i = 1;
  for (i; i < full_url.length(); i++) {
    if (full_url[i] == '/')
      break;
  }
  std::string url = full_url.substr(1,i-1); // extract uri 

  std::map<std::string,std::string>::iterator it = handlerTable_.find(url); // find if handler type exists
  if (it != handlerTable_.end()) {
    std::string name = handlerTable_.find(url)->second;
    NginxConfig* nginxconfig = configTable_.find(url)->second;
  }
  else {
    std::string name = "404"; // placeholder
    NginxConfig* nginxconfig = nullptr;
  }
  std::string path = root_;

  std::unique_ptr<RequestHandler> handler = handlermanager_.createByName(name, nginxconfig, path);
  handler.handleRequest(req);
}