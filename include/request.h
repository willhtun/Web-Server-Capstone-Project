#pragma once

#include <string>
#include <vector>
#include <memory>

typedef std::vector<std::pair<std::string,std::string>> Header_Fields;

class Request
{
public:
  // create request object using request string
  Request(std::string req);

  // function to handle the creation and parsing
  static std::unique_ptr<Request> request_handler(std::string raw_req);  
  std::string getReqRaw() const;

  // get methods
  std::string method() const;
  std::string uri_path() const;
  std::string http_version() const;

private:
  bool parse_request();
  bool check_first_request_line(std::string req_line);

  std::string req_;
  std::string method_;
  std::string uri_path_;
  std::string http_version_;
  Header_Fields header_fields_;

};