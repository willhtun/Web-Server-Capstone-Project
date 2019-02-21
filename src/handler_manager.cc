#include "handler_manager.h"

std::unique_ptr<RequestHandler> HandlerManager::createByName(const std::string& name, const NginxConfig& config, const std::string& path)
{
    if (name =="echo")
    {
        return std::unique_ptr<RequestHandler>(EchoHandler::create(config,path));
    }
    if (name =="static")
    {
        return std::unique_ptr<RequestHandler>(StaticHandler::create(config,path));
    }
    if (name =="error")
    {
        return std::unique_ptr<RequestHandler>(ErrorHandler::create(config,path));
    }
    if (name =="status")
    {
        return std::unique_ptr<RequestHandler>(StatusHandler::create(config,path));
    }
    return nullptr;
};