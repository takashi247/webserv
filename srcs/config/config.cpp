#include "config.hpp"
#include <iostream>
#include <cstdlib>
// void ConfigError::MakeUnexpected(std::string error_msg) {
//   std::cerr << error_msg << std::endl;
//   std::exit(EXIT_FAILURE);
// }

void MakeUnexpected(std::string msg) {
  std::cerr << msg << std::endl;
  std::exit(EXIT_FAILURE);
}

int ParseInt(std::stringstream &ss, int max) {
  std::string set_value;
  int set_value_int;

  if (!std::getline(ss, set_value, ' ')) {
    return (1);
  }
  set_value_int = atoi(set_value.c_str());
  if (set_value_int > max || !ss.eof()) {
    return (1);
  }
  return (set_value_int);
}

bool ParseBool(std::stringstream &ss) {
  std::string set_value = ParseString(ss);

  if (set_value == "on") {
    return (true);
  }
  if (set_value == "off") {
    return (false);
  }
  MakeUnexpected("");
  return (true);
}

std::string ParseString(std::stringstream &ss) {
  std::string set_value;

  if (!std::getline(ss, set_value, ' ')) {
    MakeUnexpected("hello");
  }
  if (!ss.eof()) {
    MakeUnexpected("hello");
  }
  return (set_value);
}

std::vector<std::string> ParseVector(std::stringstream &ss) {
  std::string set_value;
  std::vector<std::string> vec_set_value;

  std::cout << "called!!" << std::endl;
  for (std::string set_value; getline(ss, set_value, ' ');) {
    vec_set_value.push_back(set_value);
  }
  if (vec_set_value.size() == 0) {
    MakeUnexpected("");
  }
  return (vec_set_value);
}
