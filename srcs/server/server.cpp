#include <iostream>
#include <sys/select.h>	//select
#include <unistd.h>		//close
#include <fstream>		//ifstream
#include <sstream>		//stringstream
#include <vector>
#include <string>

#include "server.hpp"
#include "httpRequestParser.hpp"

Server::Server()
{
	configs_.clear();
	{
		ServerConfig *sc = new ServerConfig();
		LocationConfig *lc = new LocationConfig();
		lc->vec_index_.push_back("index.html");
		lc->root_ = "../www/";
		sc->vec_location_config_.push_back(lc);
		configs_.push_back(std::make_pair(5000, sc));
	}
}

Server::Server(const char *conf)
{
	if (conf)
		std::cout << "conf=" << conf << std::endl;
	//この時点でconfigs_が出来上がってる
	//TODO: configで内容にエラーがあればどう返す？try catchとか？
	return ;
}

const int BUF_SIZE = 3000;//1024;
const std::string HTML_FILE = "www";
const int MAX_SESSION = 10;

void Server::run()
{

	std::vector<std::pair<int, ServerConfig*> >::iterator it = configs_.begin();
	for (; it != configs_.end(); ++it)
	{
		Socket *s = new Socket(it->first);	//リスニングポートを作成
		s->set_socket();
		sockets_.insert(std::make_pair(it->first, s));	//ポートとソケットをペアで入れる。

	}

	std::string executive_file = HTML_FILE;
//	int body_length = 0;
//	int is_file_exist;
	int accfd[MAX_SESSION];
	for(int i=0; i < MAX_SESSION; i++){
		accfd[i] = -1;
	}
	fd_set fds;
	Socket *sock = sockets_[5000];

	while(1) {
		FD_ZERO(&fds);
		FD_SET(sock->get_listenfd(), &fds);
		int width = sock->get_listenfd() + 1;

		for (int i=0; i < MAX_SESSION; i++){
			if(accfd[i] != -1){
				FD_SET(accfd[i], &fds);
				if(width < (accfd[i]+1)){
					width = accfd[i]+1;
				}
			}
		}
		if(select(width, &fds, NULL,NULL, NULL) == -1) {
			std::cout << "select() failed." << std::endl;
			break;
		}

		//新規接続が見つかった場合
		if(FD_ISSET(sock->get_listenfd(), &fds)){
			int connfd = accept(sock->get_listenfd(), (struct sockaddr*)NULL, NULL);
			bool limit_over = true;
			for(int i = 0; i < MAX_SESSION; i++) {
				if(accfd[i] == -1) {
					accfd[i] = connfd;
					limit_over = false;
					break;
				}
			}
			if(limit_over) {
				close(connfd);
				std::cout << "over max connection." << std::endl;
			}
		}

		//すでに接続したsocketからメッセージを受信
		for( int i = 0; i < MAX_SESSION; i++) {
			if(accfd[i] == -1) {
				continue;
			}
			char buf[BUF_SIZE];
			memset(buf, 0, sizeof(buf));
			if(FD_ISSET(accfd[i], &fds)) {
				std::string recv_str = "";
				ssize_t read_size = 0;

				do {    //バッファサイズの制限は嫌なので、無制限に読み込める様にする。
					read_size = recv(accfd[i], buf, sizeof(buf)-1, 0);
					if(read_size == -1) {
						std::cout << "read() failed." << std::endl;
						std::cout << "ERROR: " << errno << std::endl;
						close(accfd[i]);
						accfd[i] = -1;
						break;
					}
					if(read_size > 0) {
						recv_str.append(buf);
					}
					//読み込んだ最後が\r\n\r\nなら終了。
					if( (recv_str[recv_str.length()-4] == 0x0d) &&
					(recv_str[recv_str.length()-3] == 0x0a) &&
					(recv_str[recv_str.length()-2] == 0x0d) &&
					(recv_str[recv_str.length()-1] == 0x0a)
					){
						break;
					}
				} while (read_size > 0);
				std::cout << "***** receive message *****\n";
				std::cout << recv_str << std::endl;
				std::cout << "***** receive message finished *****\n";

				HttpRequest *request = HttpRequestParser::getHttpRequest(recv_str);
				(void)request;
#if 1	//Dummy Response
				std::vector<std::string> header;
				header.push_back("HTTP/1.1 200 OK\r\n");
				header.push_back("Content-Type: text/html; charset=UTF-8\r\n");
				header.push_back("Content-Length: 39\r\n");
				header.push_back("Connection: Keep-Alive\r\n");
				header.push_back("\r\n");
				header.push_back("<html><body>Hello,world!</body></html>");
				std::string server_response;
				int header_size = header.size();
				for (int i = 0; i < header_size; i++){
			        server_response.append(header[i].c_str());
			    }
//				std::cout << server_response << std::endl;
				if(send(accfd[i], server_response.c_str(), server_response.length(), 0) == -1){
					std::cout << "write() failed." << std::endl;
				}
#endif
				close(accfd[i]);
				accfd[i] = -1;
			}
		}
	}
	close(sock->get_listenfd());


	return ;
}