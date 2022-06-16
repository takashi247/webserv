#include "../config.hpp"
#include "../config_parser.hpp"

int main(int ac, char **av)
{
	Config config(av[1]);
	ConfigParser cp;

	cp.ParseConfigFile(config);
	config.vec_server_config_[0].PrintVal();
}
