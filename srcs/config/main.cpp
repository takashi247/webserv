#include <fstream>
#include <iostream>
#include <sstream>

#include "config.hpp"
#include "config_parser.hpp"

int main() {
  Config config;
  config.config_file_ = "test.txt";
  ConfigParser cp;

  cp.ParseConfigFile(config);
}
