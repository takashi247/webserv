#include "server.hpp"

#include <sys/select.h>  //select
#include <unistd.h>      //close

#include <iostream>
#include <string>
#include <vector>

#include "http_request.hpp"
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
    std::cout << "ポート " << it->port_ << " を監視します。\n";
  }
}

int Server::SetStartFds(fd_set *p_fds) {
  int width = 0;
  std::vector< ServerSocket >::iterator it;
  FD_ZERO(p_fds);
  for (it = sockets_.begin(); it != sockets_.end(); ++it) {
    FD_SET(it->GetListenFd(), p_fds);
    if (width < it->GetListenFd()) width = it->GetListenFd();
  }
  std::vector< ClientSocket >::iterator cit = clients_.begin();
  for (; cit < clients_.end(); ++cit) {
    FD_SET(cit->fd_, p_fds);
    if (width < cit->fd_) {
      width = cit->fd_;
    }
  }
  return width;
}

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#define BUF_LEN 256 /* バッファのサイズ */
typedef struct CLIENT_INFO {
  char hostname[BUF_LEN]; /* ホスト名 */
  char ipaddr[BUF_LEN];   /* IP アドレス */
  int port;               /* ポート番号 */
  time_t last_access;     /* 最終アクセス時刻 */
} CLIENT_INFO;
CLIENT_INFO client_info[FD_SETSIZE];
void debug_print_accept_info(int new_socket) {
  socklen_t len;
  struct hostent *peer_host;
  struct sockaddr_in peer_sin;

  len = sizeof(peer_sin);
  getpeername(new_socket, (struct sockaddr *)&peer_sin, &len);

  peer_host = gethostbyaddr((char *)&peer_sin.sin_addr.s_addr,
                            sizeof(peer_sin.sin_addr), AF_INET);
  if (!peer_host) {
    std::cout << "peer_host is null" << std::endl;
    return;
  }

  /* ホスト名 */
  strncpy(client_info[new_socket].hostname, peer_host->h_name,
          sizeof client_info[new_socket].hostname);
  /* IP アドレス */
  strncpy(client_info[new_socket].ipaddr, inet_ntoa(peer_sin.sin_addr),
          sizeof client_info[new_socket].ipaddr);
  /* ポート番号 */
  client_info[new_socket].port = ntohs(peer_sin.sin_port);
  /* 現在時刻を最終アクセス時刻として記録しておく */
  time(&client_info[new_socket].last_access);

  printf("接続: %s (%s) ポート %d  ディスクリプタ %d 番\n",
         client_info[new_socket].hostname, client_info[new_socket].ipaddr,
         client_info[new_socket].port, new_socket);
}

int Server::AcceptNewClient(const fd_set *fds) {
  std::vector< ServerSocket >::iterator it = sockets_.begin();
  for (; it != sockets_.end(); ++it) {
    if (FD_ISSET(it->GetListenFd(), fds)) {
      int connfd = accept(it->GetListenFd(), (struct sockaddr *)NULL, NULL);

      debug_print_accept_info(connfd);

      if (clients_.size() < kMaxSessionNum) {
        clients_.push_back(ClientSocket(connfd, &(*it)));
      } else {
        close(connfd);
        std::cout << "over max connection." << std::endl;
      }
    }
  }
  return 0;
}

std::string Server::ReadMessage(int *p_fd) {
  std::string recv_str = "";
  char buf[kReadBufferSize];
  memset(buf, 0, sizeof(buf));
  ssize_t read_size = 0;

  // do {  //バッファサイズの制限は嫌なので、無制限に読み込めるようにしたい
  read_size = recv(*p_fd, buf, sizeof(buf) - 1, 0);
  if (read_size == -1) {
    std::cout << "recv() failed." << std::endl;
    std::cout << "ERROR: " << strerror(errno) << std::endl;
    close(*p_fd);
    *p_fd = -1;
    // break;
    exit(1);
  }
  if (read_size > 0) {
    std::string buf_string(buf, read_size);
    recv_str.append(buf_string);
    memset(buf, 0, sizeof(buf));
  }
  // 読み込んだ最後が\r\n\r\nなら終了。
  // if ((recv_str[recv_str.length() - 4] == 0x0d) &&
  //     (recv_str[recv_str.length() - 3] == 0x0a) &&
  //     (recv_str[recv_str.length() - 2] == 0x0d) &&
  //     (recv_str[recv_str.length() - 1] == 0x0a)) {
  //   break;
  // } else if (recv_str[recv_str.length() - 1] == '-' &&
  //            recv_str[recv_str.length() - 2] == '-')
  //   break;
  // } while (read_size > 0);
  return recv_str;
}

void Server::Run() {
  fd_set fds;
  /***
   * サーバーソケットを生成
   */
  CreateServerSockets();
  while (1) {
    /***
     * 複数のリスニングソケットをfdsに設定。
     * widthは参照するfdsの幅
     */
    int width = SetStartFds(&fds);

    /***
     * 受信設定
     */
    struct timeval waitval;
    waitval.tv_sec = 2; /* 待ち時間に 2.500 秒を指定 */
    waitval.tv_usec = 500;
    if (select(width + 1, &fds, NULL, NULL, &waitval) == -1) {
      std::cout << "select() failed." << std::endl;
      break;
    }

    /***
     * 受信処理（リスニングソケット宛に新規接続）
     * 新規接続のClientSocketを生成
     */
    AcceptNewClient(&fds);
    /***
     *	受信処理（接続済みソケット宛にメッセージ受信）
     */
    std::vector< ClientSocket >::iterator it = clients_.begin();
    while (it != clients_.end()) {
      //すでに接続したClientSocketからの受信を検知
      if (FD_ISSET(it->fd_, &fds)) {
        std::string recv_str = ReadMessage(&it->fd_);
        std::cout << "***** receive message *****\n";
        std::cout << recv_str << std::endl;
        std::cout << "***** receive message finished *****\n";
        /***
         * リクエストをパース
         */
        HttpRequest request;
        HttpRequestParser::Parse(recv_str, &request);
        /***
         * レスポンスメッセージを作成
         */
        const ServerConfig *sc = config_.SelectServerConfig(
            it->parent_->host_, it->parent_->port_, request.host_name_);

        HttpResponse response(request, *sc);
        std::string server_response = response.GetResponse();
        if (send(it->fd_, server_response.c_str(), server_response.length(),
                 0) == -1) {
          std::cout << "send() failed." << std::endl;
        }
        /***
         * fdを切断
         */
        close(it->fd_);
        it = clients_.erase(it);
      } else
        ++it;
    }
  }
  /***
   * 全ソケットを閉じる
   */
  std::vector< ServerSocket >::iterator it;
  it = sockets_.begin();
  for (; it != sockets_.end(); ++it) {
    close(it->GetListenFd());
  }
  return;
}
