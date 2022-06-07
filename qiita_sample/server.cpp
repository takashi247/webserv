#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <errno.h>
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>
#include <future>
#include <thread>

#include "socket.h"

#include "http1response.h"
#include "http1header.h"
#include "http1parser.h"
#include "socket.h"



const int MAX_SESSION = 10;

int http1()
{
    std::string executive_file = HTML_FILE;
    Socket *sock = new Socket(HTTP1_PORT);
    sock->set_socket();

    int body_length = 0;
    int is_file_exist;

    int accfd[MAX_SESSION];
    for(int i=0; i < MAX_SESSION; i++){
        accfd[i] = -1;
    }
    fd_set fds;

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

        //Start reading message
        for( int i = 0; i < MAX_SESSION; i++) {
            if(accfd[i] == -1) {
                continue;
            }
            char buf[BUF_SIZE];
            memset(buf, 0, sizeof(buf));
            if(FD_ISSET(accfd[i], &fds)) {
                std::string recv_str = "";
                ssize_t read_size = 0;

                do {
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
                    if( (recv_str[recv_str.length()-4] == 0x0d) &&
                    (recv_str[recv_str.length()-3] == 0x0a) &&
                    (recv_str[recv_str.length()-2] == 0x0d) &&
                    (recv_str[recv_str.length()-1] == 0x0a)
                    ){
                        break;
                    }
                } while (read_size > 0);
                std::string path = "", path_string = "";
                std::string exe = executive_file;
                std::size_t pos = exe.rfind('/');
                if (pos != std::string::npos) {
                    exe = exe.substr(pos + 1);
                }
                path_string.clear();
                path = HTTP1_Parser::get_requestline_path(buf);
                path_string = HTTP1_Parser::argv_path_analyzer(path, executive_file.c_str(), exe.c_str());
                std::cout << "path_string : " << path_string << std::endl;
                std::ifstream output_file(path_string.c_str());
                char line[256];
                is_file_exist = output_file.fail();
                body_length = 0;
                std::vector<std::string> message_body;
                if (path == "/image.png" || path == "/favicon.ico")
                {
                    std::stringstream buffer;
                    buffer << output_file.rdbuf();  //改行や空白も全て読み込む
                    message_body.push_back(buffer.str());
                    body_length = message_body[0].size();
                } else {
                    while (output_file.getline(line, 256-1)) {
                        body_length += strlen(line);
                        message_body.push_back(std::string(line));
                    }
                }
                recv_str.append(buf);
                std::string server_response;
                std::vector<std::string> header = HTTP1_Response::make_header(3, body_length, is_file_exist, path);
                server_response = HTTP1_Response::make_response(header, message_body);
                std::cout << server_response << std::endl;
                if(send(accfd[i], server_response.c_str(), server_response.length(), 0) == -1){
                    std::cout << "write() failed." << std::endl;
                }
                output_file.close();
                close(accfd[i]);
                accfd[i] = -1;
            }
        }
   }
    close(sock->get_listenfd());
    return 0;
}

int main(int argc, char *argv[])
{
    http1();
    return 0;
}