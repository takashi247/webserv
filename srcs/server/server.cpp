#include "server.hpp"

#include <sys/select.h>  //select
#include <unistd.h>      //close

#include <iostream>
#include <string>
#include <vector>

#include "http_request_parser.hpp"
#include "http_response.hpp"

Server::Server() : config_("filename") {
  std::cout << "[Debug] Server Construct!" << std::endl;
  config_.vec_server_config_.clear();
  {
    LocationConfig lc;
    lc.vec_index_.push_back("index.html");
    lc.root_ = "../www/";

    ServerConfig sc;
    sc.vec_location_config_.push_back(lc);
    sc.port_ = 8080;
    sc.host_ = "127.0.0.1";
    sc.vec_server_names_.push_back("example.com");
    ServerConfig sc2;
    sc2.vec_location_config_.push_back(lc);
    sc2.port_ = 8080;
    sc2.host_ = "192.168.1.6";
    sc2.vec_server_names_.push_back("fugafuga.com");
    ServerConfig sc3;
    sc3.vec_location_config_.push_back(lc);
    sc3.port_ = 8080;
    sc3.host_ = "0.0.0.0";
    sc3.vec_server_names_.push_back("test.com");
    config_.vec_server_config_.push_back(sc);
    config_.vec_server_config_.push_back(sc2);
    config_.vec_server_config_.push_back(sc3);
  }
}

Server::Server(const char *conf) : config_(conf) {
  sockets_.clear();
  clients_.clear();
}

void Server::CreateServerSockets() {
  std::vector< ServerConfig >::iterator it = config_.vec_server_config_.begin();
  for (; it != config_.vec_server_config_.end(); ++it) {
    //重複するポートがないかチェック
    std::vector< ServerSocket >::iterator sit = sockets_.begin();
    bool multiple_flag = false;
    for (; sit != sockets_.end(); ++sit) {
      if (it->port_ == sit->port_ && it->host_ == sit->host_) {
        multiple_flag = true;
        break;
      }
    }
    if (multiple_flag) continue;
    sockets_.push_back(ServerSocket(it->port_, it->host_));
    sockets_.back().Init();
    std::cout << "ポート " << it->port_ << " を監視します。\n";
  }
}

int Server::SetStartFds(fd_set &r_fds, fd_set &w_fds) {
  int width = 0;
  FD_ZERO(&r_fds);
  FD_ZERO(&w_fds);

  std::vector< ServerSocket >::iterator sit;
  for (sit = sockets_.begin(); sit != sockets_.end(); ++sit) {
    FD_SET(sit->GetListenFd(), &r_fds);
    if (width < sit->GetListenFd()) {
      width = sit->GetListenFd();
    }
  }
  std::vector< ClientSocket >::iterator cit;
  for (cit = clients_.begin(); cit < clients_.end(); ++cit) {
    FD_SET(cit->fd_, &r_fds);
    FD_SET(cit->fd_, &w_fds);
    if (width < cit->fd_) {
      width = cit->fd_;
    }
  }
  return width;
}

int Server::AcceptNewClient(const fd_set &fds) {
  std::vector< ServerSocket >::iterator it = sockets_.begin();
  for (; it != sockets_.end(); ++it) {
    if (FD_ISSET(it->GetListenFd(), &fds)) {
      int connfd = accept(it->GetListenFd(), (struct sockaddr *)NULL, NULL);
      if (clients_.size() < kMaxSessionNum) {
        clients_.push_back(ClientSocket(connfd, &(*it)));
        clients_.back().Init();
      } else {
        close(connfd);
        std::cout << "over max connection." << std::endl;
      }
    }
  }
  return 0;
}

/*
 * fdから読み込み、recv_strに格納する。
 */
ssize_t ReceiveMessage(int fd, std::string &recv_str) {
  ssize_t read_size = 0;
  char buf[1024];  // kReadBufferSize];
  memset(buf, 0, sizeof(buf));
  read_size = recv(fd, buf, sizeof(buf) - 1, 0);
  // std::cout << "read_size(" << read_size << ")\n";
  if (read_size == -1) {
    std::cout << "recv() failed!!!" << std::endl;
    std::cout << "ERROR: " << errno << std::endl;
  } else if (read_size > 0) {
    std::string buf_string(buf, read_size);
    recv_str.append(buf_string);
  }
  return read_size;
}

/*
 * itから\r\nまで読み込み、Chunkサイズを返す
 */
int GetChunkSize(std::string::const_iterator *it) {
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

int ReceiveChunkedMessage(int fd, std::string &recv_msg, size_t start_pos) {
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
    it += 2;  // HttpRequestParser::kCRLFSize;
    size = GetChunkSize(&it);
  }
  return 0;
}

void Server::Run() {
  fd_set r_fds;
  fd_set w_fds;
  /***
   * サーバーソケットを生成
   */
  CreateServerSockets();
  while (1) {
    /***
     * 複数のリスニングソケットをfdsに設定。
     * widthは参照するfdsの幅
     */
    int width = SetStartFds(r_fds, w_fds);

    /***
     * 受信設定
     */
    struct timeval waitval;
    waitval.tv_sec = 2; /* 待ち時間に 2.500 秒を指定 */
    waitval.tv_usec = 500;
    if (select(width + 1, &r_fds, &w_fds, NULL, &waitval) == -1) {
      std::cout << "select() failed." << std::endl;
      break;
    }

    /***
     * 受信処理（リスニングソケット宛に新規接続）
     * 新規接続のClientSocketを生成
     */
    AcceptNewClient(r_fds);
    /***
     *	受信処理（接続済みソケット宛にメッセージ受信）
     */
    std::vector< ClientSocket >::iterator it = clients_.begin();
    while (it != clients_.end()) {
      if (it->status_ == ClientSocket::WAIT_HEADER) {
        // "\r\n\r\n"が来るまで読み続ける。
        if (FD_ISSET(it->fd_, &r_fds)) {
          size_t header_end_pos;
          if (-1 == ReceiveMessage(it->fd_, it->recv_str_)) {
            it->ChangeStatus(ClientSocket::WAIT_CLOSE);
          }
          // 先頭の改行は削除
          if (0 == it->recv_str_.find("\r\n")) {
            it->recv_str_.erase(0, 2);
          }
          header_end_pos = it->recv_str_.find("\r\n\r\n");
          // recv_strに、ヘッダ部分の読み込み完了。
          if (header_end_pos != std::string::npos) {
            it->ChangeStatus(ClientSocket::PARSE_HEADER);
          }
        }
      }
      if (it->status_ == ClientSocket::PARSE_HEADER) {
        // Header部分をParse
        HttpRequestParser::ParseHeader(it->recv_str_, &it->request_);
        std::cout << "***** receive message *****\n";
        std::cout << it->recv_str_ << std::endl;
        std::cout << "***** receive message finished *****\n";
        // Headerを見て、Bodyあるなら受信フローへ
        if ((it->request_.content_length_ != 0) || (it->request_.is_chunked_)) {
          it->ChangeStatus(ClientSocket::WAIT_BODY);
        } else {
          it->ChangeStatus(ClientSocket::CREATE_RESPONSE);
        }
      }
      if (it->status_ == ClientSocket::WAIT_BODY) {
        // MessageBodyの受信待ち
        if (FD_ISSET(it->fd_, &r_fds)) {
          size_t header_end_pos = it->recv_str_.find("\r\n\r\n");
          size_t body_start_pos =
              header_end_pos + HttpRequestParser::kSeparatorSize;
          if (it->request_.is_chunked_) {
            std::cout << "Read Chunked Message\n";
            if (-1 ==
                ReceiveChunkedMessage(it->fd_, it->recv_str_, body_start_pos)) {
              it->ChangeStatus(ClientSocket::WAIT_CLOSE);
              continue;
            }
          } else if (it->request_.content_length_ != 0) {
            ssize_t remain_length = it->request_.content_length_ -
                                    (it->recv_str_.length() - body_start_pos);
            ssize_t read_size = 0;
            while (remain_length) {
              if (-1 == (read_size = ReceiveMessage(it->fd_, it->recv_str_))) {
                it->ChangeStatus(ClientSocket::WAIT_CLOSE);
                continue;
              }
              if (remain_length < read_size) {
                std::cout << "Read Error over Content-Length\n";
                it->ChangeStatus(ClientSocket::WAIT_CLOSE);
                continue;
              }
              remain_length -= read_size;
            }
            it->request_.body_.assign(it->recv_str_.begin() + body_start_pos,
                                      it->recv_str_.end());
          }
          it->ChangeStatus(ClientSocket::CREATE_RESPONSE);
        }
      }
      if (it->status_ == ClientSocket::CREATE_RESPONSE) {
        const ServerConfig *sc = config_.SelectServerConfig(
            it->parent_->host_, it->parent_->port_, it->request_.host_name_);
        HttpResponse response(it->request_, *sc, it->info_);
        it->server_response_ = response.GetResponse();
        it->ChangeStatus(ClientSocket::WAIT_SEND);
      }
      if (it->status_ == ClientSocket::WAIT_SEND) {
        if (FD_ISSET(it->fd_, &w_fds)) {
          if (send(it->fd_, it->server_response_.c_str(),
                   it->server_response_.length(), 0) == -1) {
            std::cout << "send() failed." << std::endl;
            it->ChangeStatus(ClientSocket::WAIT_CLOSE);
          }
          //送信成功したら待ち状態に戻る
          it->Init();
        }
      }
      if (it->status_ == ClientSocket::WAIT_CLOSE) {
        close(it->fd_);
        it = clients_.erase(it);
        continue;
      }
      ++it;  //次のクライアントへ
    }
  }
  /***
   * サーバー側のソケットを閉じる
   */
  std::vector< ServerSocket >::iterator sit;
  sit = sockets_.begin();
  for (; sit != sockets_.end(); ++sit) {
    close(sit->GetListenFd());
  }
  return;
}
