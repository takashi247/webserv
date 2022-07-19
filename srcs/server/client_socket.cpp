#include "client_socket.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include <iostream>

#include "http_request_parser.hpp"
#include "http_response.hpp"

static const int kReadBufferSize = 1024;
/*
 * -1: 読み込みエラー
 * other: 送信済み文字数
 */
static ssize_t ReceiveMessage(int fd, std::string &recv_str) {
  ssize_t read_size = 0;
  char buf[kReadBufferSize + 1];
  memset(buf, 0, sizeof(buf));
  read_size = recv(fd, buf, kReadBufferSize, 0);
  if (0 < read_size) {
    std::string buf_string(buf, read_size);
    recv_str.append(buf_string);
  }
  return read_size;
}

/*
 * -1: 読み込みエラー
 * other: 送信済み文字数
 */
static int SendMessage(int fd, const char *str, size_t len) {
  ssize_t send_res = send(fd, str, len, 0);
  // server_response_.c_str(), server_response_.length(), 0);
  if (send_res == -1) {
    std::cout << "send() failed." << std::endl;
  }
  return send_res;
}

/*
 * -1 : 読み込みエラー
 * other : Chunkサイズ
 */
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
    return -1;
  }
  *it = cur + 2;  // kCRLFSize;
  return size;
}

/*
 * -1 : Body読み込みエラー
 * 0 : Body読み込み完了（続きを読むかはStatusで判断）
 * other : 残り読み込みサイズ
 */
static int ReceiveNoChunkedBody(int fd, std::string &recv_msg,
                                size_t content_length, std::string &body) {
  size_t header_end_pos = recv_msg.find("\r\n\r\n");
  size_t body_start_pos = header_end_pos + HttpRequestParser::kSeparatorSize;
  ssize_t remain_length = content_length - (recv_msg.length() - body_start_pos);
  ssize_t read_size = 0;
  if (remain_length) {
    if (-1 == (read_size = ReceiveMessage(fd, recv_msg))) {
      return -1;
    }
    remain_length -= read_size;
  }
  if (remain_length < 0) {
    std::cout << "Read Error over Content-Length\n";
    return -1;
  } else if (remain_length == 0) {
    body.assign(recv_msg.begin() + body_start_pos, recv_msg.end());
    return 0;
  }
  return remain_length;
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
  Init();
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
  parsed_pos_ = other.parsed_pos_;
  chunked_remain_size_ = other.chunked_remain_size_;
  return *this;
}

void ClientSocket::Init() {
  ChangeStatus(WAIT_HEADER);
  recv_str_.clear();
  request_.Init();
  server_response_.clear();
  time(&last_access_);
  parsed_pos_ = 0;
  chunked_remain_size_ = 0;
}

/*
 * ヘッダ部分の読み込み
 * 0 : Header読み込み完了（続きを読むかはStatusで判断）
 * 1 : Header読み込みエラー
 */
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
    parsed_pos_ = header_end_pos + HttpRequestParser::kSeparatorSize;
    chunked_remain_size_ = 0;
  }
  return 0;
}

/*
 * すでに読み込んだメッセージからChunkedされた部分をパースする
 * -1: エラー切断
 * 0 : parse完了
 * 1 : まだ読み込む
 */
int ClientSocket::ParseChunkedBody() {
  // parsed_pos_ から続きを読み始める。
  if (chunked_remain_size_ == 0) {
    if (std::string::npos == recv_str_.find("\r\n", parsed_pos_)) {
      return 1;  // サイズが無いのに、終端が見つからなければ、次を読む
    }
    std::string::const_iterator it = recv_str_.begin() + parsed_pos_;
    int chunk_size = GetChunkSize(&it);
    // chunked bodyの先頭を指す。
    parsed_pos_ =
        recv_str_.find("\r\n", parsed_pos_) + HttpRequestParser::kCRLFSize;
    if (chunk_size == -1) {  //これでも読めないならエラー
      return 1;
    } else if (chunk_size == 0) {
      // std::cout << "Completed Body[" << request_.body_ << "]\n";
      return 0;  //正常終了
    }
    // chunk_sizeと終端の\r\nを含んだ数だけ読み込む
    chunked_remain_size_ = chunk_size + HttpRequestParser::kCRLFSize;
  }
  // parsed_pos_の位置から、chunked_remain_size_分を読み込みたい
  size_t remain_size = recv_str_.size() - parsed_pos_;
  // 読み込みたいchunk_sizeと、読み込み済み残りサイズの小さい方をbodyに追加する
  size_t append_size =
      (remain_size < chunked_remain_size_) ? remain_size : chunked_remain_size_;
  request_.body_.append(recv_str_, parsed_pos_, append_size);
  // chunk最後の\r\nも追加されてるため、削除する
  size_t rfind_res = request_.body_.rfind("\r\n");
  if (rfind_res != std::string::npos &&
      rfind_res == request_.body_.size() - 2) {
    request_.body_.erase(rfind_res);
  }
  parsed_pos_ += append_size;
  chunked_remain_size_ -= append_size;
  //まだ読み込んだ分が残っていて、次のchunkをパースするため、再帰呼び出し
  if (chunked_remain_size_ == 0) {
    return ParseChunkedBody();
  }
  return 1;
}

/*
 * POSTの時のBody読み込み
 * 0 : Body読み込み完了（続きを読むかはStatusで判断）
 * 1 : Body読み込みエラー → 切断
 */
int ClientSocket::ReceiveBody() {
  ssize_t remain_len = 0;
  if (request_.content_length_ != 0) {
    remain_len = ReceiveNoChunkedBody(fd_, recv_str_, request_.content_length_,
                                      request_.body_);
    if (remain_len < 0) {
      return 1;  // Read Error で切断
    } else if (remain_len == 0) {
      ChangeStatus(ClientSocket::CREATE_RESPONSE);
    }
  } else if (request_.is_chunked_) {
    remain_len = ReceiveMessage(fd_, recv_str_);
    if (-1 == remain_len) {
      return 1;
    }
    int res = ParseChunkedBody();
    if (res == -1)  //エラー
      return 1;
    else if (res == 0)  //読み込み完了
      ChangeStatus(ClientSocket::CREATE_RESPONSE);
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
    if (request_.method_ == "POST") {
      if (request_.content_length_ != 0) {
        ChangeStatus(ClientSocket::WAIT_BODY);
      } else if (request_.is_chunked_) {
        int parse_res = ParseChunkedBody();
        if (parse_res == -1)
          ChangeStatus(ClientSocket::WAIT_CLOSE);
        else if (parse_res == 0)
          ChangeStatus(ClientSocket::CREATE_RESPONSE);
        else
          ChangeStatus(ClientSocket::WAIT_BODY);
      } else {
        ChangeStatus(ClientSocket::CREATE_RESPONSE);
      }
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
    if (response.GetConnection() == "close") {
      request_.is_bad_request_ = true;
    }
    ChangeStatus(ClientSocket::WAIT_SEND);
  }
  if (status_ == ClientSocket::WAIT_SEND) {
    if ((is_writable && (SendMessage(fd_, server_response_.c_str(),
                                     server_response_.length()) < 0)) ||
        request_.is_bad_request_) {
      ChangeStatus(ClientSocket::WAIT_CLOSE);
    } else {
      Init();
    }
  }
  // timeout
  if (time(NULL) - kDisconnectSec > last_access_) {
    std::cout << info_.hostname_ << " (" << info_.ipaddr_ << ") ポート "
              << info_.port_ << " ディスクリプタ " << fd_ << "番から"
              << kDisconnectSec << "秒以上アクセスがないので切断します。\n";
    ChangeStatus(ClientSocket::WAIT_CLOSE);
  }
  if (status_ == ClientSocket::WAIT_CLOSE) {
    close(fd_);
    return 1;
  }
  return 0;
}
