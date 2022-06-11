#include "location_config.hpp"
#include "server_config.hpp"
#include <iostream>

int main()
{
	LocationConfig lc;
	ServerConfig sc(lc);

	std::cout << sc.host_ << std::endl;
}
