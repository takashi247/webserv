#include <cstddef>

#include "../config.hpp"
#include "../config_parser.hpp"

int main()
{
	const size_t port = 8080;
	Config config;
	ServerConfig sc1;
	ServerConfig sc2;
	ServerConfig sc3;

	sc1.port_ = port;
	sc1.vec_server_names_.push_back("localhost");
	sc1.map_error_page_path_.insert(std::pair<int, std::string>(200, "1"));

	sc2.port_ = port;
	sc2.map_error_page_path_.insert(std::pair<int, std::string>(200, "2"));

	sc3.map_error_page_path_.insert(std::pair<int, std::string>(200, "3"));

	config.vec_server_config_.push_back(sc1);
	config.vec_server_config_.push_back(sc2);
	config.vec_server_config_.push_back(sc3);

	const ServerConfig *res1 = config.SelectServerConfig("", port, "localhost");
	std::cout << res1->map_error_page_path_.at(200) << std::endl;
	// expecting 1

	const ServerConfig *res2 = config.SelectServerConfig("", port, "127.0.0.1");
	std::cout << res2->map_error_page_path_.at(200) << std::endl;
	// expecting 2

	const ServerConfig *res3 = config.SelectServerConfig("", port + 2, "127.0.0.1");
	std::cout << res3->map_error_page_path_.at(200) << std::endl;
	// expecting 1
}
