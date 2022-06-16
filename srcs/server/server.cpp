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
    ServerConfig *sc = new ServerConfig();
    LocationConfig *lc = new LocationConfig();
    lc->vec_index_.push_back("index.html");
    lc->root_ = "../www/";
    sc->vec_location_config_.push_back(lc);
    config_.vec_server_config_.push_back(std::make_pair(5000, sc));
    config_.vec_server_config_.push_back(std::make_pair(4242, sc));
  }
}

Server::Server(const char *conf) : config_(conf) {
  if (conf) {
    std::cout << "conf=" << conf << std::endl;
  }
  //この時点でconfigs_が出来上がってる
  // TODO: configで内容にエラーがあればどう返す？try catchとか？
  return;
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

const int BUF_SIZE = 3000;  // 1024;
const std::string HTML_FILE = "www";
const int MAX_SESSION = 10;

void Server::Run() {
  std::vector<std::pair<int, ServerConfig *> >::iterator it =
      config_.vec_server_config_.begin();
  for (; it != config_.vec_server_config_.end(); ++it) {
    Socket *s = new Socket(it->first);  //リスニングポートを作成
    s->SetSocket();
    sockets_.push_back(s);
    std::cout << "ポート " << it->first << " を監視します。\n";
  }

  std::string executive_file = HTML_FILE;
  //	int body_length = 0;
  //	int is_file_exist;
  int accfd[MAX_SESSION];
  for (int i = 0; i < MAX_SESSION; i++) {
    accfd[i] = -1;
  }
  //   int server_num = configs_.size();
  //   int i = 0;
  //   while (i < server_num) {
  fd_set fds;
  Socket *sock = sockets_[0];

  while (1) {
    // 1つのリスニングソケットを設定する。widthは参照するfdの幅・個数
    FD_ZERO(&fds);
    FD_SET(sock->GetListenFd(), &fds);
    int width = sock->GetListenFd() + 1;
    // 2周目以降は、accfd[]に監視するfdが入っている。
    // セットしつつ、fdの最大幅を更新する。
    // indexは接続順。できるだけ手前に入るはず。
    for (int i = 0; i < MAX_SESSION; i++) {
      if (accfd[i] != -1) {
        FD_SET(accfd[i], &fds);
        if (width < (accfd[i] + 1)) {
          width = accfd[i] + 1;
        }
      }
    }
    //この時点で、fdsは、1つのリスニングポートと複数の接続済みポートのフラグが立つ。
    struct timeval waitval; /* select に待ち時間を指定するための構造体 */
    waitval.tv_sec = 2; /* 待ち時間に 2.500 秒を指定 */
    waitval.tv_usec = 500;
    if (select(width, &fds, NULL, NULL, &waitval) == -1) {
      std::cout << "select() failed." << std::endl;
      break;
    }

    // 1つのリスニングポートに、新規接続が見つかった場合
    if (FD_ISSET(sock->GetListenFd(), &fds)) {
      int connfd = accept(sock->GetListenFd(), (struct sockaddr *)NULL, NULL);

      debug_print_accept_info(connfd);

      bool limit_over = true;
      for (int i = 0; i < MAX_SESSION; i++) {
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

    //すでに接続したsocketからメッセージを受信
    for (int i = 0; i < MAX_SESSION; i++) {
      if (accfd[i] == -1) {
        continue;
      }
      char buf[BUF_SIZE];
      memset(buf, 0, sizeof(buf));
      if (FD_ISSET(accfd[i], &fds)) {
        std::string recv_str = "";
        ssize_t read_size = 0;

        do {  //バッファサイズの制限は嫌なので、無制限に読み込める様にする。
          read_size = recv(accfd[i], buf, sizeof(buf) - 1, 0);
          if (read_size == -1) {
            std::cout << "read() failed." << std::endl;
            std::cout << "ERROR: " << errno << std::endl;
            close(accfd[i]);
            accfd[i] = -1;
            break;
          }
          if (read_size > 0) {
            recv_str.append(buf);
          }
          //読み込んだ最後が\r\n\r\nなら終了。
          if ((recv_str[recv_str.length() - 4] == 0x0d) &&
              (recv_str[recv_str.length() - 3] == 0x0a) &&
              (recv_str[recv_str.length() - 2] == 0x0d) &&
              (recv_str[recv_str.length() - 1] == 0x0a)) {
            break;
          }
        } while (read_size > 0);
        std::cout << "***** receive message *****\n";
        std::cout << recv_str << std::endl;
        std::cout << "***** receive message finished *****\n";

        HttpRequest *request = HttpRequestParser::CreateHttpRequest(recv_str);
        HttpResponse response(request, config_.vec_server_config_[0].second);
        std::string server_response = response.GetResponse();
        //	std::cout << server_response << std::endl;
        if (send(accfd[i], server_response.c_str(), server_response.length(),
                 0) == -1) {
          std::cout << "write() failed." << std::endl;
        }
        HttpRequestParser::DestroyHttpRequest(request);
        close(accfd[i]);
        accfd[i] = -1;
      }
    }
  }
  close(sock->GetListenFd());

  //     if (++i == server_num) i = 0;
  //   }

  return;
}