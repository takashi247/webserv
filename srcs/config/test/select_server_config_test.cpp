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
	sc1.error_page_path_ = "1";

	sc2.port_ = port;
	sc2.error_page_path_ = "2";

	sc3.error_page_path_ = "3";

	config.vec_server_config_.push_back(sc1);
	config.vec_server_config_.push_back(sc2);
	config.vec_server_config_.push_back(sc3);

	ServerConfig *res1 = config.SelectServerConfig("", port, "localhost");
	std::cout << res1->error_page_path_ << std::endl;
	// expecting 1

	ServerConfig *res2 = config.SelectServerConfig("", port, "127.0.0.1");
	std::cout << res2->error_page_path_ << std::endl;
	// expecting 2

	ServerConfig *res3 = config.SelectServerConfig("", port + 2, "127.0.0.1");
	std::cout << res3->error_page_path_ << std::endl;
	// expecting 1
}
