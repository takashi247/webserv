#include "http_response.hpp"

const std::string HttpResponse::kServerVersion = "webserv/1.0.0";
const std::string HttpResponse::kStatusDescOK = "200 OK";
const std::string HttpResponse::kStatusDescBadRequest = "400 Bad Request";
const std::string HttpResponse::kStatusDescNotFound = "404 Not Found";
const std::string HttpResponse::kStatusDescMethodNotAllowed = "405 Not Allowed";
const std::string HttpResponse::kStatusDescVersionNotSupported = "505 HTTP Version Not Supported";

HttpResponse::HttpResponse(const HttpRequest &http_request, const ServerConfig &server_config)
  : http_request_(http_request), server_config_(server_config),
    status_code_(kStatusCodeOK), status_desc_(kStatusDescOK) {
  InitResponse();
}

HttpResponse::~HttpResponse() {
  requested_file_.close();
}

void HttpResponse::InitFileStream() {
  requested_file_path_ = server_config_.UpdateUri(http_request_.uri_);
  requested_file_.open(requested_file_path_.c_str());
}

void HttpResponse::InitResponse() {
  std::ostringstream oss_date, oss_server;
  SetCurrentTime();
  oss_date << "Date: " << current_time_ << "\r\n";
  date_header_ = oss_date.str();
  oss_server << "Server: " << kServerVersion << "\r\n";
  server_header_ = oss_server.str();
  InitFileStream();
  is_file_fail_ = requested_file_.fail();
  location_config_ = server_config_.SelectLocationConfig(requested_file_path_);
}

void HttpResponse::SetCurrentTime() {
  std::time_t current = std::time(NULL);
  char buffer[100];
  if (std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&current))){
    current_time_ = std::string(buffer);
  }
}

void HttpResponse::SetLastModifiedTime() {
  struct stat result;
  char buffer[100];
  if (stat(http_request_.uri_.c_str(), &result) == 0) {
    if (std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&result.st_mtime))){
      last_modified_ = std::string(buffer);
    }
  }
}

void HttpResponse::MakeErrorHeader() {
  std::ostringstream oss_content_length;
  oss_content_length << "Content-Length: " << body_len_ << "\r\n";
  header_.push_back("HTTP/1.1 ");
  header_.push_back(status_desc_);
  header_.push_back("\r\n");
  header_.push_back(server_header_);
  header_.push_back(date_header_);
  header_.push_back("Content-Type: text/html\r\n");
  header_.push_back(oss_content_length.str());
  if (status_code_ == kStatusCodeBadRequest || status_code_  == kStatusCodeVersionNotSupported) {
    header_.push_back("Connection: close\r\n");
  } else {
    header_.push_back("Connection: keep-alive\r\n");
  }
  header_.push_back("\r\n");
}

void HttpResponse::MakeHeader200() {
  std::ostringstream oss_content_length, oss_content_type, oss_last_modified;
  oss_content_type << "Content-Type: " << http_request_.content_type_ << "\r\n";
  oss_content_length << "Content-Length: " << body_len_ << "\r\n";
  oss_last_modified << "Last-Modified: " << last_modified_ << "\r\n";
  header_.push_back("HTTP/1.1 ");
  header_.push_back(status_desc_);
  header_.push_back("\r\n");
  header_.push_back(server_header_);
  header_.push_back(date_header_);
  header_.push_back(oss_content_type.str());
  header_.push_back(oss_content_length.str());
  header_.push_back(oss_last_modified.str());
  header_.push_back("Connection: keep-alive\r\n");
  header_.push_back("Accept-Ranges: bytes\r\n");
  header_.push_back("\r\n");
}

void HttpResponse::CreateCustomizedErrorPage() {
  // TODO: Dynamically create a customized error page using error_page_path_
  return ;
}

void HttpResponse::CreateDefaultErrorPage() {
  body_ = "<html>\n"
          "<head><title>";
  body_ += status_desc_;
  body_ += "</title></head>\n"
           "<body bgcolor=\"white\">\n"
           "<center><h1>";
  body_ += status_desc_;
  body_ += "</h1></center>\n"
           "<hr><center>";
  body_ += kServerVersion;
  body_ += "</center>\n"
           "</body>\n"
           "</html>";
  body_len_ = body_.size();
}

void HttpResponse::MakeErrorBody() {
  if (server_config_.error_page_path_ == "") {
    CreateDefaultErrorPage();
  } else {
    CreateCustomizedErrorPage();
  }
}

void HttpResponse::MakeBody200() {
  std::stringstream buffer;
  buffer << requested_file_.rdbuf();
  body_ = buffer.str();
  SetLastModifiedTime();
  body_len_ = body_.size();
}

bool HttpResponse::IsAllowedMethod() const {
  std::vector<std::string>::iterator it, first, last;
  first = location_config_->vec_accepted_method_.begin();
  last = location_config_->vec_accepted_method_.end();
  it = std::find(first, last, http_request_.method_);
  return it != last;
}

// Q: How should we handle if no location config is given? >> A: Only GET is allowed
bool HttpResponse::IsValidMethod() const {
  if (http_request_.method_ == "GET" && !location_config_) {
    return true;
  } else {
    return IsAllowedMethod();
  }
}

bool HttpResponse::IsValidVersion() const {
  if (http_request_.version_ == "HTTP/1.1") {
    return true;
  } else {
    return false;
  }
}

void HttpResponse::SetStatusCode() {
  if (http_request_.is_bad_request_) {
    status_code_ = kStatusCodeBadRequest;
    status_desc_ = kStatusDescBadRequest;
  } else if (!IsValidVersion()) {
    status_code_ = kStatusCodeVersionNotSupported;
    status_desc_ = kStatusDescVersionNotSupported;
  } else if (!IsValidMethod()) {
    status_code_ = kStatusCodeMethodNotAllowed;
    status_desc_ = kStatusDescMethodNotAllowed;
  } else if (is_file_fail_) {
    status_code_ = kStatusCodeNotFound;
    status_desc_ = kStatusDescNotFound;
  }
}

int HttpResponse::MakeResponse() {
  SetStatusCode();
  switch (status_code_) {
  case kStatusCodeOK:
    MakeBody200();
    MakeHeader200();
    break;
  default:
    MakeErrorBody();
    MakeErrorHeader();
  }
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
