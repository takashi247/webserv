#include <iostream>

#include "server.hpp"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "usage: ./webserv [configuration file]" << std::endl;
    return (1);
  }
  Server server(argv[1]);
  server.Run();
  return (0);
}
