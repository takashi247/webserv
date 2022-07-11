#include "parser_utils.hpp"

void ParserUtils::MakeUnexpected(const std::string &msg, const int &pos) {
  std::ostringstream oss;

  if (pos > 0) {
    oss << "line " << pos << ": ";
  }
  oss << msg;
  throw WebservException(oss.str());
}

void ParserUtils::ParseInt(
    const std::vector<std::pair<int, std::string> > &list, size_t &i) {
  if (list.size() != 2) {
    MakeUnexpected(
        "invalid number of args in \"" + list[0].second + "\" directive",
        list[0].first);
  }
  AtoSizeT(list[1].second.c_str(), list, i);
}

void ParserUtils::ParseBool(
    const std::vector<std::pair<int, std::string> > &list, bool &b) {
  if (list.size() != 2) {
    MakeUnexpected(
        "invalid number of args in \"" + list[0].second + "\" directive",
        list[0].first);
  }
  std::string item = list[1].second;
  if (item == "on") {
    b = true;
  } else if (item == "off") {
    b = false;
  } else {
    MakeUnexpected("unexpexted arg in \"" + list[0].second +
                       "\" directive, please set on/off",
                   list[1].first);
  }
}

void ParserUtils::ParseString(
    const std::vector<std::pair<int, std::string> > &list, std::string &str) {
  if (list.size() != 2) {
    MakeUnexpected(
        "invalid number of args in \"" + list[0].second + "\" directive",
        list[0].first);
  }
  str = list[1].second;
}

void ParserUtils::ParseVector(
    const std::vector<std::pair<int, std::string> > &list,
    std::vector<std::string> &vec) {
  if (list.size() == 1) {
    MakeUnexpected(
        "invalid number of args in \"" + list[0].second + "\" directive",
        list[0].first);
  }
  // delete default value
  vec.clear();
  for (size_t i = 1; i < list.size(); ++i) {
    vec.push_back(list[i].second);
  }
}

void ParserUtils::AtoSizeT(const char *s,
                           const std::vector<std::pair<int, std::string> > &list,
                           size_t &num) {
  size_t i = 0;
  num = 0;
  const size_t size_max_mod_10 = SIZE_MAX % 10;
  const size_t size_max_div_10 = SIZE_MAX / 10;

  while (isdigit(s[i])) {
    if (num > size_max_div_10 ||
        (num == size_max_div_10 && (size_t)(s[i] - '0') > size_max_mod_10))
      MakeUnexpected(
          "invalid number specified in \"" + list[0].second + "\" directive",
          list[1].first);
    num = num * 10 + (s[i] - '0');
    i++;
  }
  if (s[i]) {
    MakeUnexpected(
        "invalid number specified in \"" + list[0].second + "\" directive",
        list[1].first);
  }
}
