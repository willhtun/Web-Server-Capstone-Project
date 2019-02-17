#include "server_status.h"
#include "request_handler.h"


//Dynamic Statuses

//Takes in the request url and reponse (200,400,404, etc) and stores it as a pair 
//TODO: determine if we should be passing in the url and response code or the actual request and response objects
void ServerStatus::insertRequest(const std::string& uri_path, Response::statuscode response) 
{
    std::pair<std::string, Response::statuscode> req(uri_path, response);
    requests_.push_back(req);
}

//counts request pairs 
int ServerStatus::CountRequests() {
    return requests_.size();
}

std::vector<std::pair<std::string, Response::statuscode>> ServerStatus::GetRequests() const 
{
    return requests_;
}




//Static Statuses


//TODO: Build functional to store the request handler types and url prefixes served by this server 
//Also need to figure out where to call this to populate these paramters. 
