#include "dispatcher.h" 
#include "handle_manager.h"
#include "request_handler.h"

Dispatcher::Dispatcher (NginxConfig* config)
{
    root_ = config->GetRoot();
    config->ParseString(0, configTable_, handlerTable_);
}

void Dispatcher::dispatch(Request *req)
{
    /*
        Determines name, and fires off HandlerManager to get a RequestHandler object.
        This object will handle the request.
    */
    HandlerManager handlermanager_;
    std::string name;
    NginxConfig* nginxconfig;
    std::string path = root_;

    // read full url
    std::string full_url = req->uri_path();
    int i = 1;
    for (i; i < full_url.length(); i++)
    {
        if (full_url[i] == '/')
        break;
    }

    // extract uri 
    std::string uri = full_url.substr(1,i-1);

    // check if handler type exists for this url
    std::map<std::string,std::string>::iterator it = handlerTable_.find(uri);
    if (it != handlerTable_.end())
    {
        // assign name
        name = handlerTable_.find(uri)->second;
        nginxconfig = configTable_.find(uri)->second;
    }
    else
    {
         // placeholder
        name = "404";
        nginxconfig = nullptr;
    }

    std::unique_ptr<RequestHandler> handler = handlermanager_.createByName(name, *nginxconfig, path);
    handler->HandleRequest(*req);
}