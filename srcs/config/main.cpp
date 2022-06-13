#include "location_config.hpp"
#include "server_config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main()
{
	LocationConfig lc;
	ServerConfig sc(lc);
	std::ifstream ifs("test.txt");
	std::vector<std::string> contents;
	std::string str;

	if (ifs.fail()) {
		perror(NULL);
		return (1);
	}

	std::stringstream ss;
	ss << ifs.rdbuf();
	sc.ParseServer(ss.str());
}
