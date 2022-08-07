#include <iostream>
#include <string>

#if 0
#include "http_request_parser.hpp"
#include "server.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "usage: ./webserv [configuration file]" << std::endl;
    return (1);
  }
  (void)argv;
  Server server;
  server.Run();
  return (0);
}

#else

#include <fstream>
#include <iostream>
#include <sstream>

#include "receive_body.hpp"
int main(int argc, char **argv) {
  if (argc != 2) {
    return (1);
  }
  std::ifstream input_file(argv[1]);
  if (!input_file.is_open()) {
    std::cerr << "Could not open the file - '" << argv[1] << "'" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string in((std::istreambuf_iterator< char >(input_file)),
                 std::istreambuf_iterator< char >());
  std::string out;
  size_t remain_size = 0;
  size_t pos = in.find("\r\n\r\n");
  if (pos != std::string::npos) {
    pos += 4;
  } else {
    std::cout << "not found\n";
    return 1;
  }
  int res = ParseChunkedBody(remain_size, in, pos, out);
  if (res != 0) {
    std::cout << "ParseChunkedBody ERROR " << res << std::endl;
    return 1;
  }
  std::cout << "body[" << out << "]" << std::endl;
}
#endif
