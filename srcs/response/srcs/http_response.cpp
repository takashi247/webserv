#include "http_response.hpp"

HttpResponse::HttpResponse(const HttpRequest &http_request, const ServerConfig &server_config)
  : http_request_(http_request), server_config_(server_config) {}

HttpResponse::~HttpResponse() {}

void HttpResponse::MakeHeader404() {
  std::ostringstream oss_content_length, oss_date;
  SetCurrentTime();
  oss_date << "Date: " << current_time_ << "\r\n";
  oss_content_length << "Content-Length: " << body_len_ << "\r\n";
  header_.push_back("HTTP/1.1 404 Not Found\r\n");
  header_.push_back("Server: webserv\r\n");
  header_.push_back(oss_date.str());
  header_.push_back("Content-Type: text/html\r\n");
  header_.push_back(oss_content_length.str());
  header_.push_back("Connection: Keep-Alive\r\n");
  header_.push_back("\r\n");
}

void HttpResponse::SetCurrentTime() {
  std::time_t current = std::time(NULL);
  char buffer[100];
  if (std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S", std::localtime(&current))){
    current_time_ = std::string(buffer);
  }
}

void HttpResponse::SetLastModifiedTime() {
  struct stat result;
  char buffer[100];
  if (stat(http_request_.uri_.c_str(), &result) == 0) {
    if (std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S", std::localtime(&result.st_mtime))){
      last_modified_ = std::string(buffer);
    }
  }
}

void HttpResponse::MakeHeader200() {
  std::ostringstream oss_content_length, oss_content_type, oss_date, oss_last_modified;
  SetCurrentTime();
  oss_content_length << "Content-Length: " << body_len_ << "\r\n";
  oss_content_type << "Content-Type: " << http_request_.content_type_ << "\r\n";
  oss_date << "Date: " << current_time_ << "\r\n";
  oss_last_modified << "Last-Modified: " << last_modified_ << "\r\n";
  header_.push_back("HTTP/1.1 200 OK\r\n");
  header_.push_back("Server: webserv\r\n");
  header_.push_back(oss_date.str());
  header_.push_back(oss_content_type.str());
  header_.push_back(oss_content_length.str());
  header_.push_back(oss_last_modified.str());
  header_.push_back("Connection: Keep-Alive\r\n");
  header_.push_back("Accept-Ranges: bytes\r\n");
  header_.push_back("\r\n");
}

bool HttpResponse::IsValidRequest() {
  if (is_file_fail_) {
    return false;
  } else {
    return true;
  }
}

int HttpResponse::MakeHeader() {
  if (!IsValidRequest()) {
    MakeHeader404();
  }
  else {
    MakeHeader200();
  }
  return EXIT_SUCCESS;
}

int HttpResponse::MakeBody() {
  std::ifstream output_file(http_request_.uri_.c_str());
  is_file_fail_ = output_file.fail();
  if (!is_file_fail_) {
    std::stringstream buffer;
    buffer << output_file.rdbuf();
    body_ = buffer.str();
    SetLastModifiedTime();
  } else {
    body_ = "<html>\n"
            "<head><title>404 Not Found</title></head>\n"
            "<body bgcolor=\"white\">\n"
            "<center><h1>404 Not Found</h1></center>\n"
            "<hr><center>nginx/1.14.2</center>\n"
            "</body>\n"
            "</html>";
  }
  body_len_ = body_.size();
  return EXIT_SUCCESS;
}

int HttpResponse::MakeResponse() {
  MakeBody();
  MakeHeader();
  for (std::vector<std::string>::iterator it = header_.begin(), end = header_.end(); it != end; ++it) {
    response_.append(*it);
  }
  response_.append(body_);
  // std::cout << "Length of response: " << GetResponseLen() << "\n" << std::endl; // for debug
  return EXIT_SUCCESS;
}

const char* HttpResponse::GetResponse() const {
  return response_.c_str();
}

size_t HttpResponse::GetResponseLen() const {
  return response_.size();
}

void HttpResponse::PrintErrorMessage(const std::string &msg) const {
  std::cerr << "Error: " << msg << std::endl;
}
