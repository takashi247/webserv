int main(int ac, char** av) {
  if (ac != 2) {
    return error message;
  }
  ConfigParser config_parser = ConfigParser(av[1]);
  std::map<int, std::vector<ServerConfig> > map_server_config = config_parser.ParseConfigFile();
  HttpServer http_server = HttpServer(map_server_config);
  http_server.RunServer();
  http_server.ShutDownServer();
  return 0;
}

int HttpServer::RunServer() {
  std::vector<Socket> vec_socket = InitSockets();
  HandleRequests(vec_socket);
}

std::vector<Socket> HttpServer::InitSockets() {
  std::vector<Socket> vec_socket;
  for (std::map<int, std::vector<ServerConfig> >::iterator map_it = map_server_config.begin(),
       map_end = map_server_config.end(); map_it != map_end; ++map_it) {
    for (std::vector<ServerConfig>::iterator vec_it = map_it->second.begin(),
         vec_end = map_it->second.end(); vec_it != vec_end; ++vec_it) {
      Socket socket = Socket(map_it->first, *vec_it);
      socket.InitSocket();
      if (nfds < socket.socket_fd + 1) {
        nfds = socket.socket_fd + 1;
      }
      vec_socket.push_back(socket);
    }
  }
  return vec_socket;
}

int HttpServer::HandleRequests(std::vector<Socket> vec_socket) {
  fd_set readfds, writefds, exceptfds;
  while (1) {
    InitFds(vec_socket, &readfds, &writefds, &exceptfds);
    int num_of_connection_reqs = select(nfds, &readfds, &writefds, &exceptfds);
    for (int i = 0, std::vector<Socket>::iterator socket_it = vec_socket.begin(), end = vec_socket.end();
         i < num_of_connection_reqs && socket_it != end; ++socket_it) {
      int res = socket_it->AcceptConnection();
      if (res == 1) {
        ++i;
        std::vector<std::string> received_message = socket_it->ReceivedRequest();
        HttpRequestParser http_request_parser = HttpRequestParser(received_message, socket_it->vec_server_config);
        HttpRequest http_request = http_request_parser.CreateHttpRequest();
        HttpResponse http_response = http_request.ProcessHttpRequest();
        socket_it->SendResponse(http_response);
        socket_it->EndConnection();
      }
    }
  }
}
