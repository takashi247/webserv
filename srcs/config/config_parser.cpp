#include "config_parser.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

ConfigParser::ConfigParser() : index_(0) {}

ConfigParser::~ConfigParser() {}

ConfigParser::ConfigParser(ConfigParser const &rhs) { *this = rhs; }

ConfigParser &ConfigParser::operator=(ConfigParser const &rhs) {
  if (this != &rhs) {
    tokens_ = rhs.tokens_;
    index_ = rhs.index_;
  }
  return (*this);
}

void ConfigParser::ParseConfigFile(Config &config) {
  Tokenize(config.config_file_.c_str());
  BalanceBraces();
  Parse(config);
}

void ConfigParser::Tokenize(const char *file_name) {
  std::ifstream ifs(file_name);

  if (ifs.fail()) {
    MakeUnexpected("failed to open config file", 0);
  }

  int line_count = 0;
  size_t pos;
  int pos_end;

  for (std::string line_buf; std::getline(ifs, line_buf);) {
    std::replace(line_buf.begin(), line_buf.end(), '\t', ' ');
    std::replace(line_buf.begin(), line_buf.end(), '\r', ' ');
    line_count++;
    pos = 0;

    while (pos < line_buf.size()) {
      switch (line_buf[pos]) {
        case ' ':
          pos = line_buf.find_first_not_of(" ", pos);
          break;
        case '{':
        case '}':
        case ';':
          tokens_.push_back(
              std::make_pair(line_count, std::string(1, line_buf[pos])));
          ++pos;
          break;
        default:
          pos_end = line_buf.find_first_of("{}; ", pos);
          tokens_.push_back(
              std::make_pair(line_count, line_buf.substr(pos, pos_end - pos)));
          pos = pos_end;
          break;
      }
    }
  }
}

void ConfigParser::BalanceBraces() {
  int depth = 0;
  size_t i = 0;

  for (; i < tokens_.size(); ++i) {
    if (tokens_[i].second == "{") {
      ++depth;
    } else if (tokens_[i].second == "}") {
      --depth;
    }

    if (depth < 0) {
      MakeUnexpected("unexpected \"}\"", tokens_[i].first);
    }
    // original rule
    if (depth > 3) {
      MakeUnexpected("unexpected \"{\", too much nesting", tokens_[i].first);
    }
  }

  if (depth > 0) {
    MakeUnexpected("unexpected end of file, expecting \"}\"", tokens_[i].first);
  }
}

void ConfigParser::Parse(Config &config) {
  for (; index_ < tokens_.size(); ++index_) {
    if (tokens_[index_].second == "server" &&
        tokens_[index_ + 1].second == "{") {
      index_ += 2;
      config.vec_server_config_.push_back(CreateServerConfig());
    } else {
      MakeUnexpected("unexpected block, expecting server",
                     tokens_[index_].first);
    }
  }
}

// include directive name to print error message
void ConfigParser::SplitIntoList(
    std::vector<std::pair<int, std::string> > &list) {
  std::string item = tokens_[index_].second;
  for (; item != ";"; ++index_, item = tokens_[index_].second) {
    if (item == "{" || item == "}") {
      MakeUnexpected("unexpected end of file, expecting \";\"",
                     tokens_[index_].first);
    }
    list.push_back(tokens_[index_]);
  }
}

ServerConfig ConfigParser::CreateServerConfig() {
  ServerConfig sc;
  std::vector<std::pair<int, std::string> > list;
  std::string item = tokens_[index_].second;

  for (; tokens_[index_].second != "}";
       ++index_, item = tokens_[index_].second) {
    // item = tokens_[index_].second;
    if (item == "location") {
      index_++;
      sc.vec_location_config_.push_back(CreateLocationConfig());
    } else {
      SplitIntoList(list);
      if (item == "listen") {
        sc.ParseListen(list);
      } else if (item == "server_name") {
        ParseVector(list, sc.vec_server_names_);
      } else if (item == "error_page") {
        ParseString(list, sc.error_page_path_);
      } else if (item == "client_max_body_size") {
        ParseInt(list, sc.client_max_body_size_);
      } else {
        MakeUnexpected("unknown directive" + tokens_[index_].second,
                       tokens_[index_].first);
      }
    }
    list.clear();
  }
  sc.PrintVal();
  return (sc);
}

LocationConfig ConfigParser::CreateLocationConfig() {
  LocationConfig lc;
  std::string item = tokens_[index_].second;

  if (item == "{" || item == "}" || item == ";") {
    MakeUnexpected("unexpected " + item + ", expecting string",
                   tokens_[index_].first);
  }
  lc.location_path_ = item;

  ++index_;
  item = tokens_[index_].second;
  if (item != "{") {
    MakeUnexpected("unexpected " + item + ", expecting string",
                   tokens_[index_].first);
  }
  ++index_;

  std::vector<std::pair<int, std::string> > list;
  item = tokens_[index_].second;
  for (; item != "}"; ++index_, item = tokens_[index_].second) {
    SplitIntoList(list);
    if (item == "http_method") {
      ParseVector(list, lc.vec_accepted_method_);
    } else if (item == "proxy_path") {
      ParseString(list, lc.proxy_pass_);
    } else if (item == "root") {
      ParseString(list, lc.root_);
    } else if (item == "autoindex") {
      ParseBool(list, lc.autoindex_);
    } else if (item == "index") {
      ParseVector(list, lc.vec_index_);
    } else if (item == "add_cgi_handler") {
      ParseVector(list, lc.vec_cgi_file_extension_);
    } else if (item == "is_uploadable") {
      ParseBool(list, lc.is_uploadable_);
    } else if (item == "upload_store") {
      ParseString(list, lc.upload_dir_);
    } else {
      MakeUnexpected(
          "unknown directive " + tokens_[index_ - list.size()].second,
          tokens_[index_ - list.size()].first);
    }
    list.clear();
  }
  return (lc);
}
