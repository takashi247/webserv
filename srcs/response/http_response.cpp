#include "http_response.hpp"

const std::string HttpResponse::kServerVersion = "webserv/1.0.0";
const std::string HttpResponse::kStatusDescOK = "200 OK";
const std::string HttpResponse::kStatusDescNoContent = "204 No Content";
const std::string HttpResponse::kStatusDescMovedPermanently =
    "301 Moved Permanently";
const std::string HttpResponse::kStatusDescBadRequest = "400 Bad Request";
const std::string HttpResponse::kStatusDescForbidden = "403 Forbidden";
const std::string HttpResponse::kStatusDescNotFound = "404 Not Found";
const std::string HttpResponse::kStatusDescMethodNotAllowed = "405 Not Allowed";
const std::string HttpResponse::kStatusDescRequestEntityTooLarge =
    "413 Request Entity Too Large";
const std::string HttpResponse::kStatusDescVersionNotSupported =
    "505 HTTP Version Not Supported";

HttpResponse::HttpResponse(const HttpRequest &http_request,
                           const ServerConfig &server_config)
    : http_request_(http_request),
      server_config_(server_config),
      status_code_(kStatusCodeOK),
      status_desc_(kStatusDescOK),
      is_bad_request_(http_request_.is_bad_request_),
      is_supported_version_(true),
      content_type_("text/html"),
      requested_file_path_(http_request_.uri_) {
  InitParameters();
  MakeResponse();
}

HttpResponse::~HttpResponse() { requested_file_.close(); }

bool HttpResponse::IsCgiFileExtension(const std::string &file_type) const {
  if (!location_config_) {
    return false;
  }
  std::vector< std::string >::const_iterator first =
      location_config_->vec_cgi_file_extension_.begin();
  std::vector< std::string >::const_iterator last =
      location_config_->vec_cgi_file_extension_.end();
  if (std::find(first, last, file_type) == last) {
    return false;
  } else {
    return true;
  }
}

void HttpResponse::SetContentType() {
  file_type_ =
      requested_file_path_.substr(requested_file_path_.find_last_of(".") + 1);
  if (file_type_ == "png") {
    content_type_ = "image/png";
  } else if (file_type_ == "jpg" || file_type_ == "jpeg") {
    content_type_ = "image/jpeg";
  } else if (file_type_ == "ico") {
    content_type_ = "image/x-icon";
  } else if (IsCgiFileExtension(file_type_)) {
    // TODO: Need to check Apache and NGINX behavior
    content_type_ = file_type_;
  }
}

void HttpResponse::ValidatePath() {
  std::string dir;
  std::stringstream ss(requested_file_path_);
  std::vector< std::string > vec_dirs;
  while (std::getline(ss, dir, '/')) {
    vec_dirs.push_back(dir);
  }
  std::string path;
  if (vec_dirs.empty()) return;
  for (std::vector< std::string >::iterator it = vec_dirs.begin();
       it != vec_dirs.end() - 1; ++it) {
    path = it == vec_dirs.begin() ? *it : path + "/" + *it;
    struct stat buffer;
    is_path_exists_ = stat(path.c_str(), &buffer) == 0;
    if (!is_path_exists_) return;
    std::ifstream ifs(path.c_str());
    is_path_forbidden_ = ifs.fail();
    if (is_path_forbidden_) return;
  }
}

bool HttpResponse::IsDirectory() const {
  struct stat s;
  if (stat(requested_file_path_.c_str(), &s) == 0) {
    if (s.st_mode & S_IFDIR) {
      return true;
    }
  }
  return false;
}

void HttpResponse::RemoveIndex(std::string &modified_path) {
  std::size_t found = modified_path.find_last_of("/");
  if (found == std::string::npos || found == modified_path.length()) {
    return;
  }
  std::string file_name = modified_path.substr(found + 1);
  for (std::vector< std::string >::const_iterator it =
           location_config_->vec_index_.begin();
       it != location_config_->vec_index_.end(); ++it) {
    if (file_name == *it) {
      modified_path.erase(found + 1);
      return;
    }
  }
}

void HttpResponse::CheckRedirection() {
  std::string modified_path =
      requested_file_path_.substr(location_config_->root_.length());
  RemoveIndex(modified_path);
  if (modified_path != http_request_.uri_) {
    is_redirected_ = true;
  } else {
    is_redirected_ = false;
  }
}

void HttpResponse::InitFileStream() {
  requested_file_path_ = server_config_.UpdateUri(http_request_.uri_);
  if (IsDirectory() &&
      requested_file_path_[requested_file_path_.length() - 1] != '/') {
    requested_file_path_ += "/";
  }
  CheckRedirection();
  if (!is_redirected_) {
    SetContentType();
    ValidatePath();
    if (is_path_exists_ && !is_path_forbidden_) {
      struct stat buffer;
      is_file_exists_ = stat(requested_file_path_.c_str(), &buffer) == 0;
      if (is_file_exists_ && content_type_ != file_type_) {
        requested_file_.open(requested_file_path_.c_str());
        is_file_forbidden_ = requested_file_.fail();
        if (requested_file_path_[requested_file_path_.length() - 1] == '/' &&
            !location_config_->autoindex_) {
          is_file_forbidden_ = true;
        }
      }
    }
  }
}

void HttpResponse::InitParameters() {
  std::ostringstream oss_date, oss_server;
  SetCurrentTime();
  oss_date << "Date: " << current_time_ << "\r\n";
  date_header_ = oss_date.str();
  oss_server << "Server: " << kServerVersion << "\r\n";
  server_header_ = oss_server.str();
  location_config_ = server_config_.SelectLocationConfig(requested_file_path_);
  InitFileStream();
}

void HttpResponse::SetCurrentTime() {
  std::time_t current = std::time(NULL);
  char buffer[100];
  if (std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT",
                    gmtime(&current))) {
    current_time_ = std::string(buffer);
  }
}

void HttpResponse::SetLastModifiedTime() {
  struct stat result;
  // TODO: Replace 100 with constant variable
  char buffer[100];
  if (stat(http_request_.uri_.c_str(), &result) == 0) {
    if (std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT",
                      gmtime(&result.st_mtime))) {
      last_modified_ = std::string(buffer);
    }
  }
}

void HttpResponse::MakeHeader301() {
  std::ostringstream oss_content_length, oss_content_type, oss_location;
  oss_content_type << "Content-Type: " << content_type_ << "\r\n";
  oss_content_length << "Content-Length: " << body_len_ << "\r\n";
  std::map< std::string, std::string >::const_iterator it_host =
      http_request_.header_fields_.find("Host");
  oss_location << "Location: http://" << it_host->second;
  std::string requested_file_path_short_ =
      requested_file_path_.substr(location_config_->root_.length());
  oss_location << requested_file_path_short_ << "\r\n";
  header_.push_back("HTTP/1.1 ");
  header_.push_back(status_desc_);
  header_.push_back("\r\n");
  header_.push_back(server_header_);
  header_.push_back(date_header_);
  header_.push_back(oss_content_type.str());
  header_.push_back(oss_content_length.str());
  header_.push_back(oss_location.str());
  header_.push_back("Connection: keep-alive\r\n");
  header_.push_back("\r\n");
}

void HttpResponse::MakeErrorHeader() {
  std::ostringstream oss_content_length, oss_content_type;
  oss_content_type << "Content-Type: " << content_type_ << "\r\n";
  oss_content_length << "Content-Length: " << body_len_ << "\r\n";
  header_.push_back("HTTP/1.1 ");
  header_.push_back(status_desc_);
  header_.push_back("\r\n");
  header_.push_back(server_header_);
  header_.push_back(date_header_);
  header_.push_back(oss_content_type.str());
  header_.push_back(oss_content_length.str());
  if (status_code_ == kStatusCodeMovedPermanently) {
    std::ostringstream oss_location;
    std::map< std::string, std::string >::const_iterator it_host =
        http_request_.header_fields_.find("Host");
    oss_location << "Location: http://" << it_host->second;
    std::string requested_file_path_short_ =
        requested_file_path_.substr(location_config_->root_.length());
    oss_location << requested_file_path_short_ << "\r\n";
    header_.push_back(oss_location.str());
  }
  if (status_code_ == kStatusCodeBadRequest ||
      status_code_ == kStatusCodeVersionNotSupported) {
    header_.push_back("Connection: close\r\n");
  } else {
    header_.push_back("Connection: keep-alive\r\n");
  }
  header_.push_back("\r\n");
}

void HttpResponse::MakeHeader204() {
  header_.push_back("HTTP/1.1 ");
  header_.push_back(status_desc_);
  header_.push_back("\r\n");
  header_.push_back(server_header_);
  header_.push_back(date_header_);
  header_.push_back("Connection: keep-alive\r\n");
  header_.push_back("\r\n");
}

void HttpResponse::MakeHeader200() {
  std::ostringstream oss_content_length, oss_content_type, oss_last_modified;
  oss_content_type << "Content-Type: " << content_type_ << "\r\n";
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
  header_.push_back(etag_header_);
  header_.push_back("Accept-Ranges: bytes\r\n");
  header_.push_back("\r\n");
}

void HttpResponse::MakeCgiHeader() {
  std::ostringstream oss_content_length;
  oss_content_length << "Content-Length: " << body_len_ << "\r\n";
  header_.push_back("HTTP/1.1 ");
  header_.push_back(status_desc_);
  header_.push_back("\r\n");
  header_.push_back(server_header_);
  header_.push_back(date_header_);
  header_.push_back(oss_content_length.str());
  header_.push_back("\r\n");
}

void HttpResponse::CreateCustomizedErrorPage() {
  // TODO: Dynamically create a customized error page using error_page_path_
  return;
}

void HttpResponse::CreateDefaultErrorPage() {
  body_ =
      "<html>\n"
      "<head><title>";
  body_ += status_desc_;
  body_ +=
      "</title></head>\n"
      "<body bgcolor=\"white\">\n"
      "<center><h1>";
  body_ += status_desc_;
  body_ +=
      "</h1></center>\n"
      "<hr><center>";
  body_ += kServerVersion;
  body_ +=
      "</center>\n"
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

void HttpResponse::DeleteRequestedFile() {
  if (remove(requested_file_path_.c_str()) != 0) {
    PrintErrorMessage("remove");
    exit(1);
  }
}

// Temporary hash function to create ETag from last modified time
// TODO: need to understand the logic

void HttpResponse::SetEtag() {
  unsigned int result;
  for (std::string::iterator it = last_modified_.begin(),
                             end = last_modified_.end();
       it != end; ++it) {
    unsigned int ch = static_cast< unsigned int >(*it);
    result = ch + (result << 4) + (result << 10) - result;
  }
  std::ostringstream oss_etag;
  oss_etag << "ETag: \"" << result << "\""
           << "\r\n";
  etag_header_ = oss_etag.str();
}

std::vector< std::string > HttpResponse::GetFileNames() {
  struct dirent *diread;
  std::vector< std::string > dir_names;
  std::vector< std::string > file_names;
  const char *dir_name = requested_file_path_.c_str();
  DIR *dir = opendir(dir_name);
  if (dir) {
    while ((diread = readdir(dir)) != NULL) {
      std::string item_name(diread->d_name);
      // TODO: Handle when the file name is longer than 1024
      if (diread->d_type == DT_DIR) {
        item_name += "/";
        dir_names.push_back(item_name);
      } else {
        file_names.push_back(item_name);
      }
    }
    closedir(dir);
  } else {
    PrintErrorMessage("opendir");
    exit(EXIT_FAILURE);  // TODO: Confirm if we can use exit
  }
  std::sort(dir_names.begin(), dir_names.end());
  std::sort(file_names.begin(), file_names.end());
  std::vector< std::string > combined;
  combined.reserve(dir_names.size() + file_names.size());
  combined.insert(combined.end(), dir_names.begin(), dir_names.end());
  combined.insert(combined.end(), file_names.begin(), file_names.end());
  return combined;
}

// File list format
// File name: "sub_dir_forbidden/                                 " 50 chars
//          : "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa..> "
// Modification date: "20-Jun-2022 02:21"
// File size: "             1325614" width is 19 chars
//          : "             1325614"

std::string HttpResponse::CreateFileList(
    std::vector< std::string > vec_file_names) {
  std::ostringstream oss;
  for (std::vector< std::string >::iterator it = vec_file_names.begin();
       it != vec_file_names.end(); ++it) {
    if ((*it)[0] == '.') continue;
    oss << "<a href=\"" << *it << "\">" << std::left << std::setw(54);
    std::string displayed_name;
    if (it->length() <= 50) {
      displayed_name = *it + "</a>";
    } else {
      displayed_name = it->substr(0, 47) + "..&gt;</a>";
    }
    oss << displayed_name;
    struct stat result;
    // TODO: Replace 100 with constant variable
    char buffer[100];
    std::string path = requested_file_path_ + *it;
    if (stat(path.c_str(), &result) == 0) {
      if (std::strftime(buffer, sizeof(buffer), "%d-%b-%Y %H:%M",
                        gmtime(&result.st_mtime))) {
        oss << " " << buffer << " ";
        if ((*it)[it->length() - 1] != '/') {
          oss << std::right << std::setw(19) << result.st_size;
        } else {  // size should be "-" if the item is a folder
          oss << std::right << std::setw(19) << "-";
        }
      }
      oss << "\n";
    }
  }
  return oss.str();
}

void HttpResponse::CreateAutoindexPage() {
  body_ =
      "<html>\n"
      "<head><title>Index of ";
  body_ += http_request_.uri_;
  body_ +=
      "</title></head>\n"
      "<body bgcolor=\"white\">\n"
      "<h1>Index of ";
  body_ += http_request_.uri_;
  body_ += "</h1><hr><pre><a href=\"../\">../</a>\n";
  std::vector< std::string > vec_file_names = GetFileNames();
  body_ += CreateFileList(vec_file_names);
  body_ +=
      "</pre><hr></body>\n"
      "</html>\n";
}

void HttpResponse::MakeBody301() { CreateDefaultErrorPage(); }

void HttpResponse::MakeBody200() {
  if (requested_file_path_[requested_file_path_.length() - 1] == '/') {
    CreateAutoindexPage();
  } else {
    std::stringstream buffer;
    buffer << requested_file_.rdbuf();
    body_ = buffer.str();
    SetLastModifiedTime();
    SetEtag();
  }
  body_len_ = body_.size();
}

bool HttpResponse::IsAllowedMethod() const {
  std::vector< std::string >::const_iterator it, first, last;
  if (!location_config_) {
    return false;
  }
  first = location_config_->vec_accepted_method_.begin();
  last = location_config_->vec_accepted_method_.end();
  it = std::find(first, last, http_request_.method_);
  return it != last;
}

// Q: How should we handle if no location config is given? >> A: Only GET is
// allowed
bool HttpResponse::IsValidMethod() const {
  if (http_request_.method_ == "GET" && !location_config_) {
    return true;
  } else {
    return IsAllowedMethod();
  }
}

bool HttpResponse::IsDigitSafe(char ch) {
  return std::isdigit(static_cast< unsigned char >(ch));
}

void HttpResponse::ValidateVersion() {
  std::string::const_iterator it = http_request_.version_.begin();
  std::string::const_iterator end = http_request_.version_.end();
  if (*it != '1') {
    is_supported_version_ = false;
    return;
  }
  ++it;
  if (IsDigitSafe(*it)) {
    is_supported_version_ = false;
    return;
  } else if (*it != '.') {
    is_bad_request_ = true;
    return;
  }
  ++it;
  while (it != end && *it == '0') {
    ++it;
  }
  int count = 0;
  while (it != end) {
    if (!IsDigitSafe(*it) || count == 3) {
      is_bad_request_ = true;
      break;
    } else {
      ++it;
      ++count;
    }
  }
}

void HttpResponse::SetStatusCode() {
  if (!is_bad_request_) {
    ValidateVersion();
  }
  if (is_bad_request_) {
    status_code_ = kStatusCodeBadRequest;
    status_desc_ = kStatusDescBadRequest;
  } else if (!is_supported_version_) {
    status_code_ = kStatusCodeVersionNotSupported;
    status_desc_ = kStatusDescVersionNotSupported;
  } else if (!IsValidMethod()) {
    status_code_ = kStatusCodeMethodNotAllowed;
    status_desc_ = kStatusDescMethodNotAllowed;
  } else if (!is_path_exists_) {
    status_code_ = kStatusCodeNotFound;
    status_desc_ = kStatusDescNotFound;
  } else if (is_path_forbidden_) {
    status_code_ = kStatusCodeForbidden;
    status_desc_ = kStatusDescForbidden;
  } else if (!is_file_exists_) {
    status_code_ = kStatusCodeNotFound;
    status_desc_ = kStatusDescNotFound;
  } else if (is_file_forbidden_) {
    status_code_ = kStatusCodeForbidden;
    status_desc_ = kStatusDescForbidden;
  } else if (http_request_.method_ == "DELETE") {
    status_code_ = kStatusCodeNoContent;
    status_desc_ = kStatusDescNoContent;
  } else if (server_config_.client_max_body_size_ <
             http_request_.content_length_) {
    status_code_ = kStatusCodeRequestEntityTooLarge;
    status_desc_ = kStatusDescRequestEntityTooLarge;
  } else if (is_redirected_) {
    status_code_ = kStatusCodeMovedPermanently;
    status_desc_ = kStatusDescMovedPermanently;
  }
}

void HttpResponse::DeleteCgiEnviron(char **cgi_env) {
  char **head = cgi_env;
  while (*cgi_env) {
    delete *cgi_env;
    ++cgi_env;
  }
  delete[] head;
}

char **HttpResponse::CreateCgiEnviron() {
  std::map< std::string, std::string > map_env;
  std::map< std::string, std::string >::const_iterator it_content_length =
      http_request_.header_fields_.find("Content-Length");
  if (it_content_length != http_request_.header_fields_.end()) {
    map_env["CONTENT_LENGTH"] = it_content_length->second;
  } else {
    map_env["CONTENT_LENGTH"] = "";
  }
  std::map< std::string, std::string >::const_iterator it_content_type =
      http_request_.header_fields_.find("Content-Type");
  if (it_content_type != http_request_.header_fields_.end()) {
    map_env["CONTENT_TYPE"] = it_content_type->second;
  }
  map_env["GATEWAY_INTERFACE"] = "CGI/1.1";
  map_env["PATH_INFO"] = http_request_.uri_;
  map_env["QUERY_STRING"] = http_request_.query_string_;
  map_env["REQUEST_METHOD"] = http_request_.method_;
  map_env["REQUEST_SCHEME"] = "http";
  map_env["REQUEST_URI"] = http_request_.uri_;
  map_env["SERVER_PORT"] = server_config_.port_;
  map_env["SERVER_PROTOCOL"] = "HTTP/1.1";
  map_env["SERVER_SOFTWARE"] = kServerVersion;
  map_env["UPLOAD_DIR"] = location_config_->upload_dir_;
  if (location_config_->is_uploadable_) {
    map_env["IS_UPLOADABLE"] = "true";
  } else {
    map_env["IS_UPLOADABLE"] = "false";
  }
  char **cgi_env = new char *[map_env.size() + 1];
  char **head = cgi_env;
  for (std::map< std::string, std::string >::const_iterator it =
           map_env.begin();
       it != map_env.end(); ++it) {
    std::string env_var = it->first + "=" + it->second;
    *cgi_env = strdup(env_var.c_str());
    if (!*cgi_env) {
      DeleteCgiEnviron(head);
      return NULL;
    }
    ++cgi_env;
  }
  *cgi_env = NULL;
  return head;
}

void HttpResponse::MakeCgiBody() {
  pid_t pid;
  char *argv[2] = {const_cast< char * >(requested_file_path_.c_str()), NULL};
  char **cgi_environ;
  int pipe_child2parent[2];
  int pipe_parent2child[2];
  char buf[kCgiBufferSize];  // TODO: Need to update
  const int READ = 0;
  const int WRITE = 1;

  if ((cgi_environ = CreateCgiEnviron()) == NULL) {
    PrintErrorMessage("CreateCgiEnviron");
    exit(1);
  }
  if (pipe(pipe_child2parent) < 0) {
    PrintErrorMessage("pipe");
    exit(1);
  }
  if (pipe(pipe_parent2child) < 0) {
    PrintErrorMessage("pipe");
    close(pipe_child2parent[READ]);
    close(pipe_child2parent[WRITE]);
    exit(1);
  }
  if ((pid = fork()) < 0) {
    PrintErrorMessage("fork");
    close(pipe_child2parent[READ]);
    close(pipe_child2parent[WRITE]);
    close(pipe_parent2child[READ]);
    close(pipe_parent2child[WRITE]);
    exit(1);
  }
  if (pid == 0) {
    dup2(pipe_parent2child[READ], STDIN_FILENO);
    dup2(pipe_child2parent[WRITE], STDOUT_FILENO);
    close(pipe_parent2child[WRITE]);
    close(pipe_child2parent[READ]);
    if (execve(argv[0], argv, cgi_environ) == -1) {
      PrintErrorMessage("execve failed");
      close(pipe_parent2child[READ]);
      close(pipe_child2parent[WRITE]);
      exit(1);
    }
  } else {
    int status;
    write(pipe_parent2child[WRITE], http_request_.body_.c_str(),
          http_request_.body_.length());
    wait(&status);
    read(pipe_child2parent[READ], buf, kCgiBufferSize);  // TODO: Need to update
    close(pipe_child2parent[WRITE]);
    close(pipe_parent2child[READ]);
    body_ = std::string(buf);
    SetLastModifiedTime();
    body_len_ = body_.size();
    SetEtag();
  }
}

void HttpResponse::MakeResponse() {
  SetStatusCode();
  switch (status_code_) {
    case kStatusCodeOK:
      if (content_type_ != file_type_) {
        MakeBody200();
        MakeHeader200();
      } else {
        MakeCgiBody();
        MakeCgiHeader();
      }
      break;
    case kStatusCodeNoContent:
      DeleteRequestedFile();
      MakeHeader204();
      break;
    case kStatusCodeMovedPermanently:
      MakeBody301();
      MakeHeader301();
    default:
      MakeErrorBody();
      MakeErrorHeader();
  }
  for (std::vector< std::string >::iterator it = header_.begin(),
                                            end = header_.end();
       it != end; ++it) {
    response_.append(*it);
  }
  response_.append(body_);
}

std::string HttpResponse::GetResponse() const { return response_; }

void HttpResponse::PrintErrorMessage(const std::string &msg) const {
  std::cerr << "Error: " << msg << std::endl;
}
