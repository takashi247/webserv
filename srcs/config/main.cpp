#include "config.hpp"

int main(int ac, char **av)
{
	(void)ac;
	Config config(av[1]);

	config.vec_server_config_[0].PrintVal();
}
