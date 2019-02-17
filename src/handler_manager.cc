#include "handle_manager.h"

std::unique_ptr<RequestHandler> createByName(const std::string& name, const NginxConfig& config, const std::string& path)
{
    if (name =="echo")
    {
        return std::unique_ptr<RequestHandler>(EchoHandler::create(config,path));
    }
    else if (name =="static")
    {
        return std::unique_ptr<RequestHandler>(StaticHandler::create(config,path));
    }
    else if (name =="error")
    {
        return std::unique_ptr<RequestHandler>(ErrorHandler::create(config,path));
    }
};