#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "../config.hpp"
#include "../config_parser.hpp"

void ExecUpdateUri(ServerConfig &sc) {
  std::cout << "[" << sc.UpdateUri("https://localhost:8080/dir") << "]"
            << std::endl;
  std::cout << "[" << sc.UpdateUri("https://localhost:8080/dir/dir/") << "]"
            << std::endl;
  std::cout << "[" << sc.UpdateUri("https://localhost:8080") << "]"
            << std::endl;
  std::cout << "[" << sc.UpdateUri("https://localhost:8080/") << "]"
            << std::endl;
  std::cout << std::endl;
}

void TestUpdateURi() {
  {
    ServerConfig sc;
    LocationConfig no_proxy;
    char *current_path = get_current_dir_name();

    no_proxy.location_path_ = "/";
    no_proxy.root_ = current_path;
    no_proxy.vec_index_.push_back("index.html");
    no_proxy.vec_index_.push_back("index.png");
    no_proxy.vec_index_.push_back("test_update_uri.cpp");

    sc.vec_location_config_.push_back(no_proxy);

    std::cout << "proxy: no, index yes, autoindex no" << std::endl;
    ExecUpdateUri(sc);

    delete current_path;
  }
  {
    ServerConfig sc;
    LocationConfig proxy;
    char *current_path = get_current_dir_name();

    proxy.location_path_ = "/";
    proxy.root_ = current_path;
    proxy.vec_index_.push_back("index.html");
    proxy.vec_index_.push_back("index.png");
    proxy.vec_index_.push_back("test_update_uri.cpp");

    proxy.proxy_pass_ = "http://localhost:8080";
    sc.vec_location_config_.push_back(proxy);

    // test
    std::cout << "proxy: yes, index no, autoindex no" << std::endl;
    ExecUpdateUri(sc);

    delete current_path;
  }
  {
    ServerConfig sc;
    LocationConfig autoindex;
    char *current_path = get_current_dir_name();

    autoindex.location_path_ = "/";
    autoindex.root_ = current_path;

    autoindex.autoindex_ = true;
    sc.vec_location_config_.push_back(autoindex);

    // test
    std::cout << "proxy: no, index yes, autoindex yes" << std::endl;
    ExecUpdateUri(sc);

    delete current_path;
  }
}

int main() { TestUpdateURi(); }
