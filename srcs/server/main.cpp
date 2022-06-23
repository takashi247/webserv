#include <iostream>
#include <string>

#include "http_request_parser.hpp"
#include "server.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "usage: ./webserv [configuration file]" << std::endl;
    return (1);
  }
  (void)argv;
  if (0) {
    std::string recv_msg(
        "P\r\n\r\n10\r\nhogehogefugafuga\r\n4\r\nxyzw\r\n0\r\n\r\n");
    std::string body;
    HttpRequestParser::GetMessageBody(recv_msg, true, body);
    return (0);
  }
  Server server;
  server.Run();
  return (0);
}
