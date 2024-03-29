#include "http_response.hpp"

const std::string HttpResponse::kServerVersion = "webserv/1.0.0";
const std::string HttpResponse::kStatusDescOK = "200 OK";
const std::string HttpResponse::kStatusDescNoContent = "204 No Content";
const std::string HttpResponse::kStatusDescMovedPermanently =
    "301 Moved Permanently";
const std::string HttpResponse::kStatusDescFound = "302 Found";
const std::string HttpResponse::kStatusDescBadRequest = "400 Bad Request";
const std::string HttpResponse::kStatusDescForbidden = "403 Forbidden";
const std::string HttpResponse::kStatusDescNotFound = "404 Not Found";
const std::string HttpResponse::kStatusDescMethodNotAllowed = "405 Not Allowed";
const std::string HttpResponse::kStatusDescRequestEntityTooLarge =
    "413 Request Entity Too Large";
const std::string HttpResponse::kStatusDescInternalServerError =
    "500 Internal Server Error";
const std::string HttpResponse::kStatusDescMethodNotImplemented =
    "501 Not Implemented";
const std::string HttpResponse::kStatusDescGatewayTimeout =
    "504 Gateway Timeout";
const std::string HttpResponse::kStatusDescVersionNotSupported =
    "505 HTTP Version Not Supported";
const std::string HttpResponse::kConnectionKeepAlive = "keep-alive";
const std::string HttpResponse::kConnectionClose = "close";
const std::map< std::string, std::string > HttpResponse::kMimeTypeMap =
    HttpResponse::CreateMimeTypeMap();

HttpResponse::HttpResponse()
    : http_request_(NULL),
      server_config_(NULL),
      client_info_(NULL),
      status_code_(kStatusCodeOK),
      status_desc_(kStatusDescOK),
      response_status_(kInitResponse),
      cgi_status_(kReadHeader),
      fd_response_read_(kFdNotSet),
      fd_cgi_read_(kFdNotSet),
      fd_cgi_write_(kFdNotSet),
      is_supported_version_(true),
      is_file_exists_(true),
      is_path_exists_(true),
      is_file_forbidden_(false),
      is_path_forbidden_(false),
      is_permanently_redirected_(false),
      is_temporarily_redirected_(false),
      content_type_("applicaton/octet-stream"),
      has_content_length_header_(false),
      is_local_redirection_(false) {}

HttpResponse::~HttpResponse() {}

void HttpResponse::Init(HttpRequest *http_request,
                        const ServerConfig *server_config,
                        t_client_info *client_info) {
  http_request_ = http_request;
  server_config_ = server_config;
  client_info_ = client_info;
  response_status_ = kOpenFile;
  is_bad_request_ = http_request_->is_bad_request_;
  original_uri_ = http_request_->uri_;
}

std::map< std::string, std::string > HttpResponse::CreateMimeTypeMap() {
  std::map< std::string, std::string > m;
  m["aac"] = "audio/aac";
  m["abw"] = "application/x-abiword";
  m["arc"] = "application/x-freearc";
  m["avi"] = "video/x-msvideo";
  m["azw"] = "application/vnd.amazon.ebook";
  m["bin"] = "application/octet-stream";
  m["bmp"] = "image/bmp";
  m["bz"] = "application/x-bzip";
  m["bz2"] = "application/x-bzip2";
  m["csh"] = "application/x-csh";
  m["css"] = "text/css";
  m["csv"] = "text/csv";
  m["doc"] = "application/msword";
  m["docx"] =
      "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
  m["eot"] = "application/vnd.ms-fontobject";
  m["epub"] = "application/epub+zip";
  m["gz"] = "application/gzip";
  m["gif"] = "image/gif";
  m["htm"] = "text/html";
  m["html"] = "text/html";
  m["ico"] = "image/vnd.microsoft.icon";
  m["ics"] = "text/calendar";
  m["jar"] = "application/java-archive";
  m["jpeg"] = "image/jpeg";
  m["jpg"] = "image/jpeg";
  m["js"] = "text/javascript";
  m["json"] = "application/json";
  m["jsonld"] = "application/ld+json";
  m["mid"] = "audio/midi";
  m["midi"] = "audio/midi";
  m["mjs"] = "text/javascript";
  m["mp3"] = "audio/mpeg";
  m["mpeg"] = "video/mpeg";
  m["mpkg"] = "application/vnd.apple.installer+xml";
  m["odp"] = "application/vnd.oasis.opendocument.presentation";
  m["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
  m["odt"] = "application/vnd.oasis.opendocument.text";
  m["oga"] = "audio/ogg";
  m["ogv"] = "video/ogg";
  m["ogx"] = "application/ogg";
  m["opus"] = "audio/opus";
  m["otf"] = "font/otf";
  m["png"] = "image/png";
  m["pdf"] = "application/pdf";
  m["php"] = "application/x-httpd-php";
  m["ppt"] = "application/vnd.ms-powerpoint";
  m["pptx"] =
      "application/"
      "vnd.openxmlformats-officedocument.presentationml.presentation";
  m["rar"] = "application/vnd.rar";
  m["rtf"] = "application/rtf";
  m["sh"] = "application/x-sh";
  m["svg"] = "image/svg+xml";
  m["swf"] = "application/x-shockwave-flash";
  m["tar"] = "application/x-tar";
  m["tif"] = "image/tiff";
  m["tiff"] = "image/tiff";
  m["ts"] = "video/mp2t";
  m["ttf"] = "font/ttf";
  m["txt"] = "text/plain";
  m["vsd"] = "application/vnd.visio";
  m["wav"] = "audio/wav";
  m["weba"] = "audio/webm";
  m["webm"] = "video/webm";
  m["webp"] = "image/webp";
  m["woff"] = "font/woff";
  m["woff2"] = "font/woff2";
  m["xhtml"] = "application/xhtml+xml";
  m["xls"] = "application/vnd.ms-excel";
  m["xlsx"] =
      "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
  m["xml"] = "application/xml";
  m["xul"] = "application/vnd.mozilla.xul+xml";
  m["zip"] = "application/zip";
  m["3gp"] = "video/3gpp";
  m["3g2"] = "video/3gpp2";
  m["7z"] = "application/x-7z-compressed";
  return m;
}

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
  if (requested_file_path_[requested_file_path_.length() - 1] == '/') {
    content_type_ = "text/html";
  } else {
    file_type_ =
        requested_file_path_.substr(requested_file_path_.find_last_of(".") + 1);
    if (IsCgiFileExtension(file_type_)) {
      content_type_ = file_type_;
    } else {
      std::map< std::string, std::string >::const_iterator it =
          kMimeTypeMap.find(file_type_);
      if (it != kMimeTypeMap.end()) {
        content_type_ = it->second;
      }
    }
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

bool HttpResponse::IsDirectory(const std::string &path) const {
  struct stat s;
  if (stat(path.c_str(), &s) == 0) {
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

void HttpResponse::ExtractPathInfo() {
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
    std::string file_type = path.substr(path.find_last_of(".") + 1);
    if (IsCgiFileExtension(file_type)) {
      struct stat buffer;
      if (stat(path.c_str(), &buffer) == 0 && (buffer.st_mode & S_IFREG)) {
        path_info_ = requested_file_path_.substr(path.length());
        path_translated_ = location_config_->root_ + path_info_;
        requested_file_path_ = path;
        return;
      }
    }

    if (!is_path_exists_) return;
    std::ifstream ifs(path.c_str());
    is_path_forbidden_ = ifs.fail();
    if (is_path_forbidden_) return;
  }
}

bool HttpResponse::IsCgiRequest() const { return content_type_ == file_type_; }

void HttpResponse::InitFileStream() {
  requested_file_path_ = server_config_->UpdateUri(original_uri_);
  if (!location_config_->rewrite_.empty()) {
    is_temporarily_redirected_ = true;
    return;
  }
  ExtractPathInfo();
  if (IsDirectory(requested_file_path_) &&
      requested_file_path_[requested_file_path_.length() - 1] != '/') {
    requested_file_path_ += "/";
    is_permanently_redirected_ = true;
    return;
  }
  SetContentType();
  ValidatePath();
  if (is_path_exists_ && !is_path_forbidden_) {
    struct stat buffer;
    is_file_exists_ = stat(requested_file_path_.c_str(), &buffer) == 0;
    if (is_file_exists_ && !IsCgiRequest()) {
      fd_response_read_ = Wrapper::Open(requested_file_path_, O_RDONLY);
      is_file_forbidden_ = fd_response_read_ == -1 ? true : false;
      if (requested_file_path_[requested_file_path_.length() - 1] == '/' &&
          !location_config_->autoindex_) {
        is_file_forbidden_ = true;
      }
    }
  }
}

void HttpResponse::InitParameters() {
  SetCurrentTime();
  date_header_ = "Date: " + current_time_ + "\r\n";
  server_header_ = "Server: " + kServerVersion + "\r\n";
  location_config_ = server_config_->SelectLocationConfig(original_uri_);
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

void HttpResponse::SetLastModifiedTime(const std::string &path) {
  struct stat result;
  char buffer[kLenOfDateBuffer];
  if (stat(path.c_str(), &result) == 0) {
    if (std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT",
                      gmtime(&result.st_mtime))) {
      last_modified_ = std::string(buffer);
    }
  }
}

bool HttpResponse::IsRequestConnectionClose() const {
  std::map< std::string, std::string >::const_iterator it_connection =
      http_request_->header_fields_.find("connection");
  if (it_connection != http_request_->header_fields_.end() &&
      it_connection->second == "close") {
    return true;
  } else {
    return false;
  }
}

void HttpResponse::MakeHeaderRedirection() {
  std::ostringstream oss_content_length, oss_content_type, oss_location;
  oss_content_type << "Content-Type: text/html\r\n";
  oss_content_length << "Content-Length: " << body_len_ << "\r\n";
  if (status_code_ == kStatusCodeMovedPermanently) {
    oss_location << "Location: http://" << client_info_->hostname_ << ":"
                 << http_request_->host_port_;
    std::string requested_file_path_short_ =
        requested_file_path_.substr(location_config_->root_.length());
    oss_location << requested_file_path_short_ << "\r\n";
  } else {
    if (requested_file_path_.find("http://") != 0) {
      oss_location << "Location: http://" << client_info_->hostname_ << ":"
                   << http_request_->host_port_;
    } else {
      oss_location << "Location: ";
    }
    oss_location << requested_file_path_ << "\r\n";
  }
  header_.push_back("HTTP/1.1 ");
  header_.push_back(status_desc_);
  header_.push_back("\r\n");
  header_.push_back(server_header_);
  header_.push_back(date_header_);
  header_.push_back(oss_content_type.str());
  header_.push_back(oss_content_length.str());
  header_.push_back(oss_location.str());
  if (IsRequestConnectionClose()) {
    header_.push_back("Connection: close\r\n");
    connection_ = kConnectionClose;
  } else {
    header_.push_back("Connection: keep-alive\r\n");
    connection_ = kConnectionKeepAlive;
  }
  header_.push_back("\r\n");
  response_status_ = kResponseIsReady;
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
  if (status_code_ == kStatusCodeMovedPermanently) {
    std::ostringstream oss_location;
    std::map< std::string, std::string >::const_iterator it_host =
        http_request_->header_fields_.find("host");
    oss_location << "Location: http://" << it_host->second;
    std::string requested_file_path_short_ =
        requested_file_path_.substr(location_config_->root_.length());
    oss_location << requested_file_path_short_ << "\r\n";
    header_.push_back(oss_location.str());
  }
  if (status_code_ == kStatusCodeBadRequest ||
      status_code_ == kStatusCodeVersionNotSupported ||
      status_code_ == kStatusCodeInternalServerError ||
      IsRequestConnectionClose()) {
    header_.push_back("Connection: close\r\n");
    connection_ = kConnectionClose;
  } else {
    header_.push_back("Connection: keep-alive\r\n");
    connection_ = kConnectionKeepAlive;
  }
  if (etag_header_.length() != 0) {
    header_.push_back(etag_header_);
  }
  header_.push_back("\r\n");
  response_status_ = kResponseIsReady;
}

void HttpResponse::MakeHeader204() {
  header_.push_back("HTTP/1.1 ");
  header_.push_back(status_desc_);
  header_.push_back("\r\n");
  header_.push_back(server_header_);
  header_.push_back(date_header_);
  if (IsRequestConnectionClose()) {
    header_.push_back("Connection: close\r\n");
    connection_ = kConnectionClose;
  } else {
    header_.push_back("Connection: keep-alive\r\n");
    connection_ = kConnectionKeepAlive;
  }
  header_.push_back("\r\n");
  response_status_ = kResponseIsReady;
}

void HttpResponse::MakeHeader200() {
  SetLastModifiedTime(requested_file_path_);
  SetEtag();
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
  if (IsRequestConnectionClose()) {
    header_.push_back("Connection: close\r\n");
    connection_ = kConnectionClose;
  } else {
    header_.push_back("Connection: keep-alive\r\n");
    connection_ = kConnectionKeepAlive;
  }
  header_.push_back(etag_header_);
  header_.push_back("Accept-Ranges: bytes\r\n");
  header_.push_back("\r\n");
  response_status_ = kResponseIsReady;
}

void HttpResponse::CreateCgiHeader() {
  std::ostringstream oss_content_length;
  oss_content_length << "Content-Length: " << body_len_ << "\r\n";
  header_.insert(header_.begin(), oss_content_length.str());
  header_.insert(header_.begin(), date_header_);
  header_.insert(header_.begin(), server_header_);
  header_.insert(header_.begin(), "\r\n");
  header_.insert(header_.begin(), status_desc_);
  header_.insert(header_.begin(), "HTTP/1.1 ");
  if (IsRequestConnectionClose()) {
    connection_ = kConnectionClose;
  } else {
    connection_ = kConnectionKeepAlive;
  }
}

int HttpResponse::OpenCustomizedErrorPage(const std::string &error_page_path) {
  std::string path = location_config_->root_ + error_page_path;
  if (IsDirectory(path)) {
    return -1;
  }
  SetLastModifiedTime(path);
  SetEtag();
  return Wrapper::Open(path, O_RDONLY);
}

ssize_t HttpResponse::ReadFdIntoBody(int fd) {
  ssize_t read_size = kReadBufferSize;
  char buf[kReadBufferSize];
  memset(buf, 0, sizeof(buf));
  read_size = Wrapper::Read(fd, buf, kReadBufferSize);
  if (read_size != -1) {
    std::string buf_string(buf, read_size);
    body_.append(buf_string);
  }
  return read_size;
}

void HttpResponse::CreateResponseBody() {
  ssize_t read_size = kReadBufferSize;
  read_size = ReadFdIntoBody(fd_response_read_);
  if (read_size == -1) {
    CloseNResetFd(fd_response_read_);
    Make500Response();
  } else if (read_size == 0) {
    body_len_ = body_.size();
    CloseNResetFd(fd_response_read_);
    response_status_ = kCreateResponseHeader;
  }
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
  response_status_ = kCreateResponseHeader;
}

void HttpResponse::MakeErrorBody() {
  if (response_status_ == kOpenFile) {
    std::map< int, std::string >::const_iterator it =
        location_config_->map_error_page_path_.find(status_code_);
    if (it != location_config_->map_error_page_path_.end()) {
      fd_response_read_ = OpenCustomizedErrorPage(it->second);
      if (fd_response_read_ == -1) {
        CreateDefaultErrorPage();
      } else {
        response_status_ = kCreateResponseBody;
      }
    } else {
      CreateDefaultErrorPage();
    }
  } else if (response_status_ == kCreateResponseBody) {
    CreateResponseBody();
  }
}

void HttpResponse::DeleteRequestedFile() {
  if (remove(requested_file_path_.c_str()) != 0) {
    Make500Response();
  } else {
    response_status_ = kResponseIsReady;
  }
}

// Hash function to create ETag from last modified time
// Read a last modifed date of the requested file character by character
// Convert the character into unsigned int and compute the sum of those numbers
// while applying bit calculation to interim results

void HttpResponse::SetEtag() {
  unsigned int result = 0;
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
      if (diread->d_type == DT_DIR) {
        item_name += "/";
        dir_names.push_back(item_name);
      } else {
        file_names.push_back(item_name);
      }
    }
    closedir(dir);
  } else {
    PrintErrorMessage("opendir failed");
    status_code_ = kStatusCodeInternalServerError;
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
    char buffer[kLenOfDateBuffer];
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
  CloseNResetFd(fd_response_read_);
  body_ =
      "<html>\n"
      "<head><title>Index of ";
  body_ += original_uri_;
  body_ +=
      "</title></head>\n"
      "<body bgcolor=\"white\">\n"
      "<h1>Index of ";
  body_ += original_uri_;
  body_ += "</h1><hr><pre><a href=\"../\">../</a>\n";
  std::vector< std::string > vec_file_names = GetFileNames();
  if (status_code_ == kStatusCodeOK) {
    body_ += CreateFileList(vec_file_names);
    body_ +=
        "</pre><hr></body>\n"
        "</html>\n";
    body_len_ = body_.size();
    response_status_ = kCreateResponseHeader;
  } else {
    Make500Response();
  }
}

void HttpResponse::MakeBodyRedirection() { CreateDefaultErrorPage(); }

void HttpResponse::MakeBody200() {
  if (requested_file_path_[requested_file_path_.length() - 1] == '/') {
    CreateAutoindexPage();
  } else {
    CreateResponseBody();
  }
}

bool HttpResponse::IsAllowedMethod() const {
  std::vector< std::string >::const_iterator it, first, last;
  if (!location_config_) {
    return false;
  }
  first = location_config_->vec_accepted_method_.begin();
  last = location_config_->vec_accepted_method_.end();
  it = std::find(first, last, http_request_->method_);
  return it != last;
}

bool HttpResponse::IsValidMethod() const {
  if (http_request_->method_ == "GET" && !location_config_) {
    return true;
  } else {
    return IsAllowedMethod();
  }
}

bool HttpResponse::IsDigitSafe(char ch) {
  return std::isdigit(static_cast< unsigned char >(ch));
}

// not much is specified regarding syntax of HTTP version (3.1 of RFC2616)
// validation logic below is mainly based on the behavior of NGINX

void HttpResponse::ValidateVersion() {
  std::string::const_iterator it = http_request_->version_.begin();
  std::string::const_iterator end = http_request_->version_.end();
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

void HttpResponse::SetStatusDescription() {
  switch (status_code_) {
    case kStatusCodeOK:
      status_desc_ = kStatusDescOK;
      break;
    case kStatusCodeNoContent:
      status_desc_ = kStatusDescNoContent;
      break;
    case kStatusCodeMovedPermanently:
      status_desc_ = kStatusDescMovedPermanently;
      break;
    case kStatusCodeFound:
      status_desc_ = kStatusDescFound;
      break;
    case kStatusCodeBadRequest:
      status_desc_ = kStatusDescBadRequest;
      break;
    case kStatusCodeForbidden:
      status_desc_ = kStatusDescForbidden;
      break;
    case kStatusCodeNotFound:
      status_desc_ = kStatusDescNotFound;
      break;
    case kStatusCodeMethodNotAllowed:
      status_desc_ = kStatusDescMethodNotAllowed;
      break;
    case kStatusCodeRequestEntityTooLarge:
      status_desc_ = kStatusDescRequestEntityTooLarge;
      break;
    case kStatusCodeInternalServerError:
      status_desc_ = kStatusDescInternalServerError;
      break;
    case kStatusCodeMethodNotImplemented:
      status_desc_ = kStatusDescMethodNotImplemented;
      break;
    case kStatusCodeGatewayTimeout:
      status_desc_ = kStatusDescGatewayTimeout;
      break;
    case kStatusCodeVersionNotSupported:
      status_desc_ = kStatusDescVersionNotSupported;
      break;
    default:
      status_desc_ = "Unknown status code";
      break;
  }
}

bool HttpResponse::IsImplementedMethod() const {
  if (http_request_->method_ == "GET" || http_request_->method_ == "POST" ||
      http_request_->method_ == "DELETE") {
    return true;
  } else {
    return false;
  }
}

void HttpResponse::SetStatusCode() {
  if (!is_bad_request_) {
    ValidateVersion();
  }
  if (is_bad_request_) {
    status_code_ = kStatusCodeBadRequest;
  } else if (!is_supported_version_) {
    status_code_ = kStatusCodeVersionNotSupported;
  } else if (!IsImplementedMethod()) {
    status_code_ = kStatusCodeMethodNotImplemented;
  } else if (!IsValidMethod()) {
    status_code_ = kStatusCodeMethodNotAllowed;
  } else if (!is_path_exists_ || !is_file_exists_) {
    status_code_ = kStatusCodeNotFound;
  } else if (is_permanently_redirected_) {
    status_code_ = kStatusCodeMovedPermanently;
  } else if (is_temporarily_redirected_) {
    status_code_ = kStatusCodeFound;
  } else if (is_path_forbidden_ || is_file_forbidden_) {
    status_code_ = kStatusCodeForbidden;
  } else if (http_request_->method_ == "DELETE") {
    status_code_ = kStatusCodeNoContent;
  } else if (location_config_->client_max_body_size_ <
             http_request_->content_length_) {
    status_code_ = kStatusCodeRequestEntityTooLarge;
  }
  if (status_code_ == kStatusCodeOK) {
    response_status_ = kCreateResponseBody;
  }
}

void HttpResponse::DeleteCgiEnviron(char **cgi_env) {
  char **head = cgi_env;
  while (*cgi_env) {
    free(*cgi_env);
    ++cgi_env;
  }
  delete[] head;
}

std::string HttpResponse::GetHeaderValue(const std::string &header_name) {
  std::map< std::string, std::string >::const_iterator it =
      http_request_->header_fields_.find(header_name);
  if (it != http_request_->header_fields_.end()) {
    return it->second;
  } else {
    return "";
  }
}

char **HttpResponse::CreateCgiEnviron() {
  std::map< std::string, std::string > map_env;
  map_env["AUTH_TYPE"] = GetHeaderValue("authorization");
  map_env["CONTENT_LENGTH"] = GetHeaderValue("content-length");
  map_env["CONTENT_TYPE"] = GetHeaderValue("content-type");
  map_env["GATEWAY_INTERFACE"] = "CGI/1.1";
  map_env["PATH_INFO"] = path_info_;
  map_env["PATH_TRANSLATED"] = path_translated_;
  map_env["QUERY_STRING"] = http_request_->query_string_;
  map_env["REMOTE_ADDR"] = client_info_->ipaddr_;
  map_env["REMOTE_HOST"] = client_info_->hostname_;
  map_env["REMOTE_PORT"] = IntegerToString< int >(client_info_->port_);
  map_env["REQUEST_METHOD"] = http_request_->method_;
  map_env["SCRIPT_NAME"] =
      path_info_.empty()
          ? original_uri_
          : original_uri_.substr(0, original_uri_.find(path_info_));
  map_env["SERVER_NAME"] = http_request_->host_name_;
  map_env["SERVER_PORT"] = IntegerToString< size_t >(server_config_->port_);
  map_env["SERVER_PROTOCOL"] = "HTTP/1.1";
  map_env["SERVER_SOFTWARE"] = kServerVersion;
  map_env["HTTP_ACCEPT"] = GetHeaderValue("accept");
  map_env["HTTP_FORWARDED"] = GetHeaderValue("forwarded");
  map_env["HTTP_REFERER"] = GetHeaderValue("referer");
  map_env["HTTP_USER_AGENT"] = GetHeaderValue("user-agent");
  map_env["HTTP_X_FORWARDED_FOR"] = GetHeaderValue("x-forwarded-for");
  map_env["X_UPLOAD_DIR"] = location_config_->upload_dir_;
  map_env["X_IS_UPLOADABLE"] =
      location_config_->is_uploadable_ ? "true" : "false";
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

int HttpResponse::ExtractStatusCode(const std::string &header_value) {
  if (header_value.length() < kLenOfStatusCode) {
    return kStatusCodeInternalServerError;
  } else if (kLenOfStatusCode < header_value.length()) {
    if (header_value[kLenOfStatusCode] != ' ' &&
        header_value[kLenOfStatusCode] != '\r') {
      return kStatusCodeInternalServerError;
    }
  }
  std::string status_code_str = header_value.substr(0, kLenOfStatusCode);
  for (std::string::const_iterator it = status_code_str.begin();
       it != status_code_str.end(); ++it) {
    if (!IsDigitSafe(*it)) return kStatusCodeInternalServerError;
  }
  return StringToInteger< int >(status_code_str);
}

std::string HttpResponse::GetFieldValue(const std::string &header_field) {
  std::string field_value;
  size_t pos_colon = header_field.find(":");
  if (pos_colon != std::string::npos) {
    field_value = header_field.substr(pos_colon + 1);
    if (!field_value.empty()) {
      size_t pos_non_space = field_value.find_first_not_of(" ");
      field_value = (pos_non_space == std::string::npos)
                        ? ""
                        : field_value.substr(pos_non_space);
      // remove \r\n
      field_value = field_value.substr(0, field_value.length() - 2);
    }
  }
  return field_value;
}

void HttpResponse::ParseCgiHeader() {
  size_t found;
  bool has_content_type_header = false;
  while (1) {
    found = body_.find("\r\n");
    std::string header_field = body_.substr(0, found + 2);
    header_.push_back(header_field);
    body_ = body_.substr(header_field.length());
    if (header_field == "\r\n") {
      if (!is_local_redirection_ && !has_content_type_header) {
        status_code_ = kStatusCodeInternalServerError;
      }
      return;
    }
    std::string field_name = header_field.substr(0, header_field.find(":"));
    std::string field_value = GetFieldValue(header_field);
    if (field_value.empty()) continue;
    if (field_name == "Content-Type") {
      has_content_type_header = true;
    } else if (field_name == "Status") {
      int status_code = ExtractStatusCode(field_value);
      if (status_code != kStatusCodeOK) {
        status_code_ = status_code;
        return;
      }
    } else if (field_name == "Location") {
      if (field_value == original_uri_) {
        status_code_ = kStatusCodeInternalServerError;
      } else if (field_value[0] == '/') {  // in case of local redirection
        is_local_redirection_ = true;
        original_uri_ = field_value;
      } else {
        status_code_ = kStatusCodeFound;
        requested_file_path_ = field_value;
        return;
      }
    } else if (field_name == "Content-Length") {
      has_content_length_header_ = true;
      body_len_ = StringToInteger< size_t >(field_value);
    }
  }
}

bool HttpResponse::CreateCgiBody(bool has_content_length, bool is_read_finish) {
  bool is_body_completed = false;
  if (!has_content_length) {
    if (is_read_finish) {
      body_len_ = body_.size();
      is_body_completed = true;
    }
  } else if (body_.size() == body_len_) {
    is_body_completed = true;
  }
  return is_body_completed;
}

void HttpResponse::Make500Response() {
  status_code_ = kStatusCodeInternalServerError;
  header_.clear();
  body_.clear();
  response_status_ = kOpenFile;
  MakeResponse();
}

void HttpResponse::Make504Response() {
  status_code_ = kStatusCodeGatewayTimeout;
  header_.clear();
  body_.clear();
  response_status_ = kOpenFile;
  MakeResponse();
}

int HttpResponse::SendRequestBody(int fd,
                                  const HttpRequest &http_request) const {
  if (http_request.body_.length() != 0 &&
      Wrapper::Write(fd, http_request.body_.c_str(),
                     http_request.body_.length()) <= 0) {
    return 0;
  }
  return 1;
}

void HttpResponse::PrepareCgiResponse() {
  char *argv[2] = {const_cast< char * >(requested_file_path_.c_str()), NULL};
  char **cgi_environ;

  if ((cgi_environ = CreateCgiEnviron()) == NULL) {
    return Make500Response();
  }
  if (Wrapper::Pipe(pipe_child2parent_) < 0) {
    DeleteCgiEnviron(cgi_environ);
    return Make500Response();
  }
  if (Wrapper::Pipe(pipe_parent2child_) < 0) {
    CloseNResetFd(pipe_child2parent_[kReadFd]);
    CloseNResetFd(pipe_child2parent_[kWriteFd]);
    DeleteCgiEnviron(cgi_environ);
    return Make500Response();
  }
  if ((cgi_pid_ = Wrapper::Fork()) < 0) {
    CloseNResetFd(pipe_child2parent_[kReadFd]);
    CloseNResetFd(pipe_child2parent_[kWriteFd]);
    CloseNResetFd(pipe_parent2child_[kReadFd]);
    CloseNResetFd(pipe_parent2child_[kWriteFd]);
    DeleteCgiEnviron(cgi_environ);
    return Make500Response();
  }
  if (cgi_pid_ == 0) {
    alarm(kCgiTimeout);
    if (Wrapper::Dup2(pipe_parent2child_[kReadFd], STDIN_FILENO) == -1 ||
        Wrapper::Dup2(pipe_child2parent_[kWriteFd], STDOUT_FILENO) == -1 ||
        CloseNResetFd(pipe_parent2child_[kWriteFd]) == -1 ||
        CloseNResetFd(pipe_child2parent_[kReadFd]) == -1 ||
        Wrapper::Execve(argv[0], argv, cgi_environ) == -1) {
      CloseNResetFd(pipe_parent2child_[kReadFd]);
      CloseNResetFd(pipe_child2parent_[kWriteFd]);
      DeleteCgiEnviron(cgi_environ);
      exit(EXIT_FAILURE);
    }
  } else {
    DeleteCgiEnviron(cgi_environ);
    if (CloseNResetFd(pipe_child2parent_[kWriteFd]) == -1) {
      return Make500Response();
    }
    fd_cgi_read_ = pipe_child2parent_[kReadFd];
    fd_cgi_write_ = pipe_parent2child_[kWriteFd];
    response_status_ = kWriteCgiInput;
  }
}

void HttpResponse::WriteRequestBody() {
  int wstatus;

  if (SendRequestBody(pipe_parent2child_[kWriteFd], *http_request_) == 0) {
    CloseNResetFd(pipe_parent2child_[kReadFd]);
    CloseNResetFd(pipe_parent2child_[kWriteFd]);
    CloseNResetFd(pipe_child2parent_[kReadFd]);
    fd_cgi_read_ = kFdNotSet;
    fd_cgi_write_ = kFdNotSet;
    return Make500Response();
  }
  if (Wrapper::Waitpid(cgi_pid_, &wstatus, 0) == -1 ||
      (WIFSIGNALED(wstatus) && WTERMSIG(wstatus) == SIGALRM)) {
    CloseNResetFd(pipe_parent2child_[kReadFd]);
    CloseNResetFd(pipe_parent2child_[kWriteFd]);
    CloseNResetFd(pipe_child2parent_[kReadFd]);
    fd_cgi_read_ = kFdNotSet;
    fd_cgi_write_ = kFdNotSet;
    return Make504Response();
  }
  response_status_ = kReadCgiOutput;
}

void HttpResponse::ReadCgiOutput() {
  ssize_t read_size = kReadBufferSize;
  read_size = ReadFdIntoBody(pipe_child2parent_[kReadFd]);
  if (read_size == -1) {
    cgi_status_ = kCloseConnection;
    status_code_ = kStatusCodeInternalServerError;
  }
  if (cgi_status_ == kReadHeader) {
    size_t header_end = body_.find("\r\n\r\n");
    if (header_end != std::string::npos) {
      cgi_status_ = kParseHeader;
    } else if (header_end == std::string::npos && read_size == 0) {
      status_code_ = kStatusCodeInternalServerError;
      cgi_status_ = kCloseConnection;
    }
  }
  if (cgi_status_ == kParseHeader) {
    ParseCgiHeader();
    if (is_local_redirection_ &&
        ((header_.size() != 2) ||
         (has_content_length_header_ && !body_.empty()) ||
         (!has_content_length_header_ && read_size == 0))) {
      status_code_ = kStatusCodeInternalServerError;
    }
    cgi_status_ = (status_code_ == kStatusCodeOK && !is_local_redirection_)
                      ? kCreateBody
                      : kCloseConnection;
  }
  if (cgi_status_ == kCreateBody &&
      CreateCgiBody(has_content_length_header_, read_size == 0)) {
    cgi_status_ = kCloseConnection;
  }
  if (cgi_status_ == kCloseConnection) {
    fd_cgi_read_ = kFdNotSet;
    fd_cgi_write_ = kFdNotSet;
    if (CloseNResetFd(pipe_parent2child_[kReadFd]) == -1 ||
        CloseNResetFd(pipe_parent2child_[kWriteFd]) == -1 ||
        CloseNResetFd(pipe_child2parent_[kReadFd]) == -1) {
      return Make500Response();
    }
    if (status_code_ != kStatusCodeOK || is_local_redirection_) {
      header_.clear();
      body_.clear();
      response_status_ = kOpenFile;
      if (is_local_redirection_) {
        InitParameters();
        SetStatusCode();
      }
      MakeResponse();
    } else {
      CreateCgiHeader();
      for (std::vector< std::string >::iterator it = header_.begin(),
                                                end = header_.end();
           it != end; ++it) {
        response_.append(*it);
      }
      response_.append(body_);
      response_status_ = kResponseIsReady;
    }
  }
}

void HttpResponse::MakeResponse() {
  SetStatusDescription();
  switch (status_code_) {
    case kStatusCodeOK:
      MakeBody200();
      if (status_code_ == kStatusCodeOK &&
          response_status_ == kCreateResponseHeader) {
        MakeHeader200();
      }
      break;
    case kStatusCodeNoContent:
      MakeHeader204();
      DeleteRequestedFile();
      break;
    case kStatusCodeMovedPermanently:
      MakeBodyRedirection();
      MakeHeaderRedirection();
      break;
    case kStatusCodeFound:
      MakeBodyRedirection();
      MakeHeaderRedirection();
      break;
    default:
      MakeErrorBody();
      if (response_status_ == kCreateResponseHeader) {
        MakeErrorHeader();
      }
  }
  if (response_status_ == kResponseIsReady) {
    for (std::vector< std::string >::iterator it = header_.begin(),
                                              end = header_.end();
         it != end; ++it) {
      response_.append(*it);
    }
    response_.append(body_);
  }
}

std::string HttpResponse::GetResponse() const { return response_; }

std::string HttpResponse::GetConnection() const { return connection_; }

void HttpResponse::Reset() {
  http_request_ = NULL;
  server_config_ = NULL;
  client_info_ = NULL;
  status_code_ = kStatusCodeOK;
  status_desc_ = kStatusDescOK;
  response_status_ = kInitResponse;
  cgi_status_ = kReadHeader;
  fd_response_read_ = kFdNotSet;
  fd_cgi_read_ = kFdNotSet;
  fd_cgi_write_ = kFdNotSet;
  is_supported_version_ = true;
  is_file_exists_ = true;
  is_path_exists_ = true;
  is_file_forbidden_ = false;
  is_path_forbidden_ = false;
  is_permanently_redirected_ = false;
  is_temporarily_redirected_ = false;
  content_type_ = "application/octet-stream";
  original_uri_.clear();
  has_content_length_header_ = false;
  is_local_redirection_ = false;
  requested_file_path_.clear();
  location_config_ = NULL;
  path_info_.clear();
  path_translated_.clear();
  server_header_.clear();
  date_header_.clear();
  etag_header_.clear();
  header_.clear();
  body_.clear();
  body_len_ = 0;
  response_.clear();
  current_time_.clear();
  last_modified_.clear();
  file_type_.clear();
  connection_.clear();
  pipe_child2parent_[kReadFd] = 0;
  pipe_child2parent_[kWriteFd] = 0;
  pipe_parent2child_[kReadFd] = 0;
  pipe_parent2child_[kWriteFd] = 0;
  cgi_pid_ = 0;
}

int HttpResponse::GetResponseReadFd() const { return fd_response_read_; }

int HttpResponse::GetCgiReadFd() const { return fd_cgi_read_; }

int HttpResponse::GetCgiWriteFd() const { return fd_cgi_write_; }

bool HttpResponse::IsWritingCgiInput() const {
  return response_status_ == kWriteCgiInput;
}

int HttpResponse::Create(bool is_readable, bool is_cgi_readable,
                         bool is_cgi_writable) {
  if (is_readable == false && is_cgi_readable == false &&
      is_cgi_writable == false && response_status_ == kOpenFile) {
    InitParameters();
    SetStatusCode();
    if (status_code_ != kStatusCodeOK ||
        requested_file_path_[requested_file_path_.length() - 1] == '/') {
      MakeResponse();
    } else if (IsCgiRequest()) {
      PrepareCgiResponse();
    }
  } else if (is_readable == true && response_status_ == kCreateResponseBody) {
    MakeResponse();
  } else if (is_cgi_writable == true && response_status_ == kWriteCgiInput) {
    WriteRequestBody();
  } else if (is_cgi_readable == true && response_status_ == kReadCgiOutput) {
    ReadCgiOutput();
  }
  return response_status_ == kResponseIsReady ? 1 : 0;
}

int HttpResponse::CloseNResetFd(int &fd) {
  int res = Wrapper::Close(fd);
  fd = kFdNotSet;
  return res;
}

void HttpResponse::PrintErrorMessage(const std::string &msg) const {
#ifdef LOG
  std::cerr << "Error: " << msg << std::endl;
#else
  (void)msg;
#endif
}
