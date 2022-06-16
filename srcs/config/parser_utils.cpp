#include "parser_utils.hpp"

void ParserUtils::MakeUnexpected(const std::string &msg, const int &pos) {
  if (pos > 0) {
    std::cerr << "line " << pos << ": ";
  }
  std::cerr << msg << std::endl;
  std::exit(1);
}

// need max ??
// [TODO] overflow, minus
void ParserUtils::ParseInt(
    const std::vector<std::pair<int, std::string> > &list, size_t &i) {
  if (list.size() != 2) {
    MakeUnexpected("invalid number of args in server_name directive",
                   list[0].first);
  }
  i = atoi(list[1].second.c_str());
}

void ParserUtils::ParseBool(
    const std::vector<std::pair<int, std::string> > &list, bool &b) {
  if (list.size() != 2) {
    MakeUnexpected("invalid number of args in " + list[0].second + " directive",
                   list[0].first);
  }
  std::string item = list[1].second;
  if (item == "on") {
    b = true;
  } else if (item == "off") {
    b = false;
  } else {
    MakeUnexpected("unexpexted arg in " + list[0].second +
                       " directive, please set with on/off",
                   0);
  }
}

void ParserUtils::ParseString(
    const std::vector<std::pair<int, std::string> > &list, std::string &str) {
  if (list.size() != 2) {
    MakeUnexpected("invalid number of args in " + list[0].second + " directive",
                   list[0].first);
  }
  str = list[1].second;
}

void ParserUtils::ParseVector(
    const std::vector<std::pair<int, std::string> > &list,
    std::vector<std::string> &vec) {
  if (list.size() == 1) {
    MakeUnexpected("invalid number of args in " + list[0].second + " directive",
                   list[0].first);
  }
  // delete default value
  vec.clear();
  for (size_t i = 1; i < list.size(); ++i) {
    vec.push_back(list[i].second);
  }
}
