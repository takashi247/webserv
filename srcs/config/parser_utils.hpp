#ifndef PARSER_UTILS_HPP_
#define PARSER_UTILS_HPP_

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
// it is like c because cstdint is not supported in c++98
#include <stdint.h>
#include <sstream>
#include "webserv_exception.hpp"

class ParserUtils
{
 public:
  static void MakeUnexpected(const std::string& msg, const int& pos);
  static void ParseInt(const std::vector<std::pair<int, std::string> >& list,
                size_t& i);
  static void ParseBool(const std::vector<std::pair<int, std::string> >& list,
                 bool& b);
  static void ParseString(const std::vector<std::pair<int, std::string> >& list,
                   std::string& str);
  static void ParseVector(const std::vector<std::pair<int, std::string> >& list,
                   std::vector<std::string>& str);
  static void AtoSizeT(const char *s, const std::vector<std::pair<int, std::string> > &list, size_t &num);
};

#endif
