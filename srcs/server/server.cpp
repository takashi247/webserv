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
  config_.vec_server_config_.clear();
  {
    LocationConfig lc;
    lc.vec_index_.push_back("index.html");
    lc.root_ = "../www/";

    ServerConfig sc;
    sc.vec_location_config_.push_back(lc);
    sc.port_ = 5000;
    ServerConfig sc2;
    sc.vec_location_config_.push_back(lc);
    sc2.port_ = 4242;

    config_.vec_server_config_.push_back(sc);
    config_.vec_server_config_.push_back(sc2);
  }
}

Server::Server(const char *conf) : config_(conf) {}

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

int Server::AcceptNewClient(const fd_set *fds, int *accfd) {
  std::vector< Socket * >::iterator it = sockets_.begin();
  for (; it != sockets_.end(); ++it) {
    if (FD_ISSET((*it)->GetListenFd(), fds)) {
      int connfd = accept((*it)->GetListenFd(), (struct sockaddr *)NULL, NULL);
      debug_print_accept_info(connfd);

      bool limit_over = true;
      for (int i = 0; i < kMaxSessionNum; i++) {
        if (accfd[i] == -1) {
          accfd[i] = connfd;
          limit_over = false;
          break;
        }
      }
      if (limit_over) {
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

  do {  //バッファサイズの制限は嫌なので、無制限に読み込めるようにしたい
    read_size = recv(*p_fd, buf, sizeof(buf) - 1, 0);
    if (read_size == -1) {
      std::cout << "read() failed." << std::endl;
      std::cout << "ERROR: " << errno << std::endl;
      close(*p_fd);
      *p_fd = -1;
      break;
    }
    if (read_size > 0) {
      recv_str.append(buf);
      memset(buf, 0, sizeof(buf));
    }
    //読み込んだ最後が\r\n\r\nなら終了。
    if ((recv_str[recv_str.length() - 4] == 0x0d) &&
        (recv_str[recv_str.length() - 3] == 0x0a) &&
        (recv_str[recv_str.length() - 2] == 0x0d) &&
        (recv_str[recv_str.length() - 1] == 0x0a)) {
      break;
    }
  } while (read_size > 0);
  return recv_str;
}

void Server::Run() {
  /***
   * 全ソケットを生成
   */
  {
    std::vector< ServerConfig >::iterator it =
        config_.vec_server_config_.begin();
    for (; it != config_.vec_server_config_.end(); ++it) {
      Socket *s = new Socket(it->port_);
      s->SetSocket();
      sockets_.push_back(s);
      std::cout << "ポート " << it->port_ << " を監視します。\n";
    }
  }
  int accfd[kMaxSessionNum];
  for (int i = 0; i < kMaxSessionNum; i++) {
    accfd[i] = -1;
  }

  fd_set fds;
  std::vector< Socket * >::iterator it;

  while (1) {
    /***
     * 複数のリスニングソケットをfdsに設定。
     * widthは参照するfdsの幅
     */
    FD_ZERO(&fds);
    int width = 0;
    it = sockets_.begin();
    for (; it != sockets_.end(); ++it) {
      FD_SET((*it)->GetListenFd(), &fds);
      if (width < ((*it)->GetListenFd() + 1)) width = (*it)->GetListenFd() + 1;
    }

    // 2周目以降は、accfd[]に接続済みのfdが入っている。
    // iは接続順で、できるだけ手前に入る。
    for (int i = 0; i < kMaxSessionNum; i++) {
      if (accfd[i] != -1) {
        FD_SET(accfd[i], &fds);
        if (width < (accfd[i] + 1)) {
          width = accfd[i] + 1;
        }
      }
    }

    /***
     * 受信設定
     */
    //この時点で、fdsは、リスニングfdと複数の接続済みfdにフラグが立つ。
    struct timeval waitval;
    waitval.tv_sec = 2; /* 待ち時間に 2.500 秒を指定 */
    waitval.tv_usec = 500;
    if (select(width, &fds, NULL, NULL, &waitval) == -1) {
      std::cout << "select() failed." << std::endl;
      break;
    }

    /***
     * 受信処理（リスニングソケット宛に新規接続）
     * 新規接続をaccfdに登録
     */
    AcceptNewClient(&fds, accfd);

    /***
     *	受信処理（接続済みソケット宛にメッセージ受信）
     */
    for (int i = 0; i < kMaxSessionNum; i++) {
      if (accfd[i] == -1) {
        continue;
      }
      //すでに接続したsocketからの受信を検知
      if (FD_ISSET(accfd[i], &fds)) {
        std::string recv_str = ReadMessage(&accfd[i]);
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
        HttpResponse response(request, config_.vec_server_config_[0]);
        std::string server_response = response.GetResponse();
        if (send(accfd[i], server_response.c_str(), server_response.length(),
                 0) == -1) {
          std::cout << "write() failed." << std::endl;
        }
        /***
         * fdを切断
         */
        close(accfd[i]);
        accfd[i] = -1;
      }
    }
  }
  /***
   * 全ソケットを閉じる
   */
  it = sockets_.begin();
  for (; it != sockets_.end(); ++it) {
    close((*it)->GetListenFd());
  }

  return;
}