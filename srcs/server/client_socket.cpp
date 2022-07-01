#include "client_socket.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include <iostream>

#include "http_request_parser.hpp"
#include "http_response.hpp"

static const int kReadBufferSize = 1024;
static const int kCRLFSize = 2;
/*
 * fdから読み込み、recv_strに格納する。
 */
static ssize_t ReceiveMessage(int fd, std::string &recv_str) {
  ssize_t read_size = 0;
  char buf[kReadBufferSize];
  memset(buf, 0, sizeof(buf));
  read_size = recv(fd, buf, sizeof(buf) - 1, 0);
  if (read_size == -1) {
    std::cout << "recv() failed!!!" << std::endl;
    std::cout << "ERROR: " << errno << std::endl;
  } else if (read_size > 0) {
    std::string buf_string(buf, read_size);
    recv_str.append(buf_string);
  }
  return read_size;
}

static int GetChunkSize(std::string::const_iterator *it) {
  std::string::const_iterator cur = *it;
  int size = 0;
  while (('0' <= *cur && *cur <= '9') || ('A' <= *cur && *cur <= 'F')) {
    size *= 16;
    if ('0' <= *cur && *cur <= '9')
      size += *cur - '0';
    else if ('A' <= *cur && *cur <= 'F')
      size += *cur - 'A' + 10;
    ++cur;
  }
  if (*cur != '\r' || *(cur + 1) != '\n') {
    std::cout << "Unexpected Separator" << std::endl;
    return 0;
  }
  *it = cur + 2;  // kCRLFSize;
  return size;
}

static int ReceiveChunkedMessage(int fd, std::string &recv_msg,
                                 size_t start_pos) {
  // body読み込み済み\r\nがあるかチェック
  if (std::string::npos == recv_msg.find("\r\n", start_pos)) {
    std::cout << "ReceiveMsg.Chunk!!!\t";
    if (-1 == ReceiveMessage(fd, recv_msg)) {
      return -1;
    }
  }
  std::string::const_iterator it = recv_msg.begin() + start_pos;
  int size = GetChunkSize(&it);
  if (size == 0) {  //これでも読めないならエラー
    return -1;
  }
  while (0 < size) {
    it += size;
    if (*it != '\r' || *(it + 1) != '\n') {
      std::cout << "Unexpected Separator" << std::endl;
      break;
    }
    it += kCRLFSize;
    size = GetChunkSize(&it);
  }
  return 0;
}

ClientSocket::ClientSocket(int fd, const ServerSocket *parent,
                           struct sockaddr_in &sin)
    : fd_(fd), parent_(parent) {
  SetNonBlocking(fd_);
  struct hostent *peer_host;
  if (!(peer_host = gethostbyaddr((char *)&sin.sin_addr.s_addr,
                                  sizeof(sin.sin_addr), AF_INET))) {
    std::cout << "peer_host is null" << std::endl;
    return;
  }
  info_.hostname_.assign(peer_host->h_name);
  info_.ipaddr_.assign(inet_ntoa(sin.sin_addr));
  info_.port_ = ntohs(sin.sin_port);

  std::cout << "接続: " << info_.hostname_ << "(" << info_.ipaddr_ << ")ポート "
            << info_.port_ << " ディスクリプタ " << fd << " 番\n";
}

ClientSocket::ClientSocket(const ClientSocket &other) { *this = other; }

ClientSocket &ClientSocket::operator=(const ClientSocket &other) {
  status_ = other.status_;
  fd_ = other.fd_;
  parent_ = other.parent_;
  info_ = other.info_;
  recv_str_ = other.recv_str_;
  request_ = other.request_;
  server_response_ = other.server_response_;
  last_access_ = other.last_access_;
  return *this;
}

void ClientSocket::Init() {
  ChangeStatus(WAIT_HEADER);
  recv_str_.clear();
  request_.Init();
  server_response_.clear();
  time(&last_access_);
}

int ClientSocket::ReceiveHeader() {
  size_t header_end_pos;
  if (-1 == ReceiveMessage(fd_, recv_str_)) {
    return 1;
  }
  // 先頭の改行は削除
  if (0 == recv_str_.find("\r\n")) {
    recv_str_.erase(0, 2);
  }
  header_end_pos = recv_str_.find("\r\n\r\n");
  if (header_end_pos != std::string::npos) {
    ChangeStatus(ClientSocket::PARSE_HEADER);
  }
  return 0;
}

int ClientSocket::ReceiveBody() {
  size_t header_end_pos = recv_str_.find("\r\n\r\n");
  size_t body_start_pos = header_end_pos + HttpRequestParser::kSeparatorSize;
  if (request_.is_chunked_) {
    std::cout << "Read Chunked Message\n";
    if (-1 == ReceiveChunkedMessage(fd_, recv_str_, body_start_pos)) {
      return 1;
    }
  } else if (request_.content_length_ != 0) {
    ssize_t remain_length =
        request_.content_length_ - (recv_str_.length() - body_start_pos);
    ssize_t read_size = 0;
    // TODO
    // telnetからのPOSTなど、bodyが断片的にくる場合は分割して受け取れるようにしないと
    while (remain_length) {
      if (-1 == (read_size = ReceiveMessage(fd_, recv_str_))) {
        return 1;
      }
      if (remain_length < read_size) {
        std::cout << "Read Error over Content-Length\n";
        return 1;
      }
      remain_length -= read_size;
    }
    request_.body_.assign(recv_str_.begin() + body_start_pos, recv_str_.end());
  }
  ChangeStatus(ClientSocket::CREATE_RESPONSE);
  return 0;
}

int ClientSocket::SendMessage() {
  if (send(fd_, server_response_.c_str(), server_response_.length(), 0) == -1) {
    std::cout << "send() failed." << std::endl;
    return 1;
  }
  return 0;
}

int ClientSocket::EventHandler(bool is_readable, bool is_writable,
                               Config &config) {
  if (status_ == ClientSocket::WAIT_HEADER) {
    if (is_readable && ReceiveHeader()) {
      ChangeStatus(ClientSocket::WAIT_CLOSE);
    }
  }
  if (status_ == ClientSocket::PARSE_HEADER) {
    HttpRequestParser::ParseHeader(recv_str_, &request_);
    std::cout << "***** receive message *****\n";
    std::cout << recv_str_ << std::endl;
    std::cout << "***** receive message finished *****\n";
    if ((request_.content_length_ != 0) || (request_.is_chunked_)) {
      ChangeStatus(ClientSocket::WAIT_BODY);
    } else {
      ChangeStatus(ClientSocket::CREATE_RESPONSE);
    }
  }
  if (status_ == ClientSocket::WAIT_BODY) {
    if (is_readable && ReceiveBody()) {
      ChangeStatus(ClientSocket::WAIT_CLOSE);
    }
  }
  if (status_ == ClientSocket::CREATE_RESPONSE) {
    const ServerConfig *sc = config.SelectServerConfig(
        parent_->host_, parent_->port_, request_.host_name_);
    HttpResponse response(request_, *sc, info_);
    server_response_ = response.GetResponse();
    // TODO:HttpRequestでBadRequestになった場合、sendしてからCloseするのに取得
    // request_.is_bad_request_ = response.GetBadRequest();
    ChangeStatus(ClientSocket::WAIT_SEND);
  }
  if (status_ == ClientSocket::WAIT_SEND) {
    if ((is_writable && SendMessage()) || request_.is_bad_request_) {
      ChangeStatus(ClientSocket::WAIT_CLOSE);
      std::cout << "WAIT_SEND bad_request!!!" << std::endl;
    } else {
      Init();
    }
  }
  if (status_ == ClientSocket::WAIT_CLOSE) {
    close(fd_);
    return 1;
  }
  return 0;
}
