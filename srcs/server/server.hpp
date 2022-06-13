#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>

# include "config.hpp"
# include "socket.hpp"


class Server
{
private:
	std::vector<std::pair<int, ServerConfig*> > configs_;
	std::map<int, Socket*> sockets_;
public:
	Server();	//削除予定
	Server(const char *conf);
	~Server() {};

	void run();
};

#endif
