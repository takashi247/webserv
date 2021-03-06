#include "server.hpp"
#include "webserv_exception.hpp"

#include <sys/select.h>  //select
#include <unistd.h>      //close

#include <iostream>
#include <string>
#include <vector>

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

Server::Server(const char *conf) try : config_(conf) {
  sockets_.clear();
  clients_.clear();
} catch (const WebservException &e) {
  std::cerr << e.what() << std::endl;
  std::exit(1);
}

void Server::CreateServerSockets() {
  std::vector< ServerConfig >::iterator it = config_.vec_server_config_.begin();
  sockets_.reserve(config_.vec_server_config_.size());
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
    try {
      sockets_.back().Init();
    } catch (const WebservException &e) {
      std::cerr << e.what() << std::endl;
      std::exit(EXIT_FAILURE);
    }
    std::cout << "ポート " << it->port_ << " を監視します。\n";
  }
}

int Server::SetStartFds(fd_set &r_fds, fd_set &w_fds) {
  int width = 0;
  int cur_fd;
  FD_ZERO(&r_fds);
  FD_ZERO(&w_fds);

  std::vector< ServerSocket >::iterator sit;
  for (sit = sockets_.begin(); sit != sockets_.end(); ++sit) {
    cur_fd = sit->GetListenFd();
    FD_SET(cur_fd, &r_fds);
    width = (width < cur_fd) ? cur_fd : width;
  }
  std::vector< ClientSocket >::iterator cit;
  for (cit = clients_.begin(); cit < clients_.end(); ++cit) {
    cur_fd = cit->GetFd();
    FD_SET(cur_fd, &r_fds);
    FD_SET(cur_fd, &w_fds);
    width = (width < cur_fd) ? cur_fd : width;
  }
  return width;
}

int Server::AcceptNewClient(const fd_set &fds) {
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);
  std::vector< ServerSocket >::iterator it = sockets_.begin();
  for (; it != sockets_.end(); ++it) {
    if (FD_ISSET(it->GetListenFd(), &fds)) {
      int connfd = accept(it->GetListenFd(), (struct sockaddr *)&sin, &len);
      // TODO 最大接続数を10にした理由を課題分やレビューから見つける。
      if (clients_.size() < kMaxSessionNum) {
        clients_.push_back(ClientSocket(connfd, &(*it), sin));
        clients_.back().Init();
      } else {
        close(connfd);
        std::cout << "over max connection." << std::endl;
      }
    }
  }
  return 0;
}

void Server::Run() {
  int width;
  fd_set r_fds;
  fd_set w_fds;
  struct timeval waitval;
  /***
   * サーバーソケットを生成
   */
  CreateServerSockets();
  while (1) {
    /***
     * 複数のリスニングソケットをfdsに設定。
     * widthは参照するfdsの幅
     */
    width = SetStartFds(r_fds, w_fds);

    /***
     * 受信設定
     */
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
      if (it->EventHandler(FD_ISSET(it->GetFd(), &r_fds),
                           FD_ISSET(it->GetFd(), &w_fds), config_)) {
        it = clients_.erase(it);
        continue;
      }
      ++it;
    }
  }

  // TODO:  後処理自体はメモリリークしないことだけ確認したい
  std::vector< ClientSocket >::iterator cit;
  for (cit = clients_.begin(); cit != clients_.end(); ++cit) {
    close(cit->GetFd());
  }
  std::vector< ServerSocket >::iterator sit;
  for (sit = sockets_.begin(); sit != sockets_.end(); ++sit) {
    close(sit->GetListenFd());
  }
  return;
}
