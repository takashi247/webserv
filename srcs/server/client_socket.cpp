#include "client_socket.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include <iostream>

#include "http_request_parser.hpp"
#include "receive_body.hpp"
#include "wrapper.hpp"

static const int kReadBufferSize = 1024;
static const int kSeparatorSize = 4;

static std::string SizeToStr(size_t size) {
  std::stringstream ss;
  ss << size;
  return ss.str();
}

/*
 * -1: 読み込みエラー
 * other: 読み込み済み文字数
 */
static ssize_t ReceiveMessage(int fd, std::string &recv_str) {
  char buf[kReadBufferSize + 1];
  memset(buf, 0, sizeof(buf));
  ssize_t read_size = Wrapper::Recv(fd, buf, kReadBufferSize);
  if (0 < read_size) {
    std::string buf_string(buf, read_size);
    recv_str.append(buf_string);
  }
  return read_size;
}

ClientSocket::ClientSocket(int fd, const ServerSocket *parent,
                           struct sockaddr_in &sin)
    : fd_(fd), parent_(parent) {
  SetNonBlocking(fd_);
  struct hostent *peer_host;
  if (!(peer_host = gethostbyaddr((char *)&sin.sin_addr.s_addr,
                                  sizeof(sin.sin_addr), AF_INET))) {
    Wrapper::PrintMsg("peer_host is null");
    return;
  }
  info_.hostname_.assign(peer_host->h_name);
  info_.ipaddr_.assign(inet_ntoa(sin.sin_addr));
  info_.port_ = ntohs(sin.sin_port);
  Init();
#ifdef LOG
  std::cout << "Connection to " << info_.hostname_ << "(" << info_.ipaddr_
            << ") port:" << info_.port_ << " descriptor:" << fd << ".\n";
#endif
}

void ClientSocket::Init() {
  ChangeStatus(WAIT_HEADER);
  recv_str_.clear();
  request_.Init();
  server_response_.clear();
  time(&last_access_);
  parsed_pos_ = 0;
  remain_size_ = 0;
}

/*
 * ヘッダ部分の読み込み
 * 0 : Header読み込み完了（続きを読むかはStatusで判断）
 * 1 : Header読み込みエラー
 */
int ClientSocket::ReceiveHeader() {
  size_t header_end_pos;
  if (0 >= ReceiveMessage(fd_, recv_str_)) {
    return 1;
  }
  // 先頭の改行は削除
  if (0 == recv_str_.find("\r\n")) {
    recv_str_.erase(0, 2);
  }
  header_end_pos = recv_str_.find("\r\n\r\n");
  if (header_end_pos != std::string::npos) {
    ChangeStatus(ClientSocket::PARSE_HEADER);
    parsed_pos_ = header_end_pos + kSeparatorSize;
    remain_size_ = 0;
  }
  return 0;
}

/*
 * POSTの時のBody読み込み
 * 0 : Body読み込み完了（続きを読むかはStatusで判断）
 * 1 : Body読み込みエラー → 切断
 */
int ClientSocket::ReceiveBody() {
  if (0 >= ReceiveMessage(fd_, recv_str_)) {
    return 1;
  }
  if (request_.content_length_ != 0) {
    int res =
        ParseNormalBody(remain_size_, recv_str_, parsed_pos_, request_.body_);
    if (res < 0)
      return 1;
    else if (res == 0) {
      ChangeStatus(ClientSocket::INIT_RESPONSE);
    }
  } else if (request_.is_chunked_) {
    int res =
        ParseChunkedBody(remain_size_, recv_str_, parsed_pos_, request_.body_);
    if (res < 0)
      return 1;
    else if (res == 0) {  //読み込み完了
      request_.header_fields_["content-length"] =
          SizeToStr(request_.body_.size());
      ChangeStatus(ClientSocket::INIT_RESPONSE);
    }
  }
  return 0;
}

int ClientSocket::EventHandler(t_fd_acceptable &ac, Config &config) {
  if (status_ == ClientSocket::WAIT_HEADER && ac.client_read) {
    if (ReceiveHeader()) {
      ChangeStatus(ClientSocket::WAIT_CLOSE);
    }
    ac.client_read = false;
    ac.client_write = false;
  }
  if (status_ == ClientSocket::PARSE_HEADER) {
    HttpRequestParser::ParseHeader(recv_str_, &request_);
    // std::cout << "***** receive message *****\n";
    // std::cout << recv_str_ << std::endl;
    // std::cout << "***** receive message finished *****\n";
    if (request_.method_ == "POST") {
      if (request_.content_length_ != 0) {
        remain_size_ = request_.content_length_;
        int res = ParseNormalBody(remain_size_, recv_str_, parsed_pos_,
                                  request_.body_);
        if (res == -1)
          ChangeStatus(ClientSocket::WAIT_CLOSE);
        else if (res == 0)
          ChangeStatus(ClientSocket::INIT_RESPONSE);
        else
          ChangeStatus(ClientSocket::WAIT_BODY);
      } else if (request_.is_chunked_) {
        int parse_res = ParseChunkedBody(remain_size_, recv_str_, parsed_pos_,
                                         request_.body_);
        if (parse_res == -1)
          ChangeStatus(ClientSocket::WAIT_CLOSE);
        else if (parse_res == 0) {
          request_.header_fields_["content-length"] =
              SizeToStr(request_.body_.size());
          ChangeStatus(ClientSocket::INIT_RESPONSE);
        } else
          ChangeStatus(ClientSocket::WAIT_BODY);
      } else {
        ChangeStatus(ClientSocket::INIT_RESPONSE);
      }
    } else {
      ChangeStatus(ClientSocket::INIT_RESPONSE);
    }
  }

  if (status_ == ClientSocket::WAIT_BODY && ac.client_read) {
    if (ReceiveBody()) {
      ChangeStatus(ClientSocket::WAIT_CLOSE);
    }
    ac.client_read = false;
    ac.client_write = false;
  }

  if (status_ == ClientSocket::INIT_RESPONSE) {
    const ServerConfig *sc = config.SelectServerConfig(
        parent_->host_, parent_->port_, request_.host_name_);
    response_.Init(&request_, sc, &info_);
    ChangeStatus(ClientSocket::CREATE_RESPONSE);
  }
  if (status_ == ClientSocket::CREATE_RESPONSE) {
    if (response_.Create(ac.response_read, ac.cgi_read, ac.cgi_write)) {
      server_response_ = response_.GetResponse();
      if (response_.GetConnection() == "close") {
        request_.is_bad_request_ = true;
      }
      remain_size_ = server_response_.length();
      ChangeStatus(ClientSocket::WAIT_SEND);
      ac.client_write = false;
    }
  }
  if (status_ == ClientSocket::WAIT_SEND && ac.client_write) {
    {
      ssize_t start_pos = server_response_.length() - remain_size_;
      ssize_t send_size = Wrapper::Send(
          fd_, server_response_.c_str() + start_pos, remain_size_);
      if (0 >= send_size) {
        ChangeStatus(ClientSocket::WAIT_CLOSE);
      } else {
        remain_size_ -= send_size;
        if (remain_size_ == 0) {  // 送り切ったら切断・もしくは初期化
          response_.Reset();
          if (request_.is_bad_request_) {
            ChangeStatus(ClientSocket::WAIT_CLOSE);
          } else
            Init();
        }
      }
      usleep(5000);
    }
  }
  // timeout
  if (time(NULL) - kDisconnectSec > last_access_) {
#ifdef LOG
    std::cout << "No access from " << info_.hostname_ << "(" << info_.ipaddr_
              << ") port:" << info_.port_ << " descriptor:" << fd_
              << " for more than " << kDisconnectSec
              << " second, so it disconnects.\n";
#endif
    ChangeStatus(ClientSocket::WAIT_CLOSE);
  }
  if (status_ == ClientSocket::WAIT_CLOSE) {
    if (0 == Wrapper::Close(fd_)) {
#ifdef LOG
      std::cout << "Disconnect descriptor:" << fd_ << ".\n";
#endif
    }
    return 1;
  }
  return 0;
}
