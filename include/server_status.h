#include "response.h"
#include <string>


class ServerStatus
{
  public: 
    //Request Methods
    void insertRequest(const std::string& uri_path, Response::statuscode response);
    int CountRequests();
    std::vector<std::pair<std::string, Response::statuscode>> GetRequests() const;

  private: 
    std::vector<std::pair<std::string, Response::statuscode>> requests_;
}