#include <cstddef>

#include "../config.hpp"
#include "../config_parser.hpp"

template <typename T>
void assert(const T &got, const T &want, const char *msg) {
  if (got != want) {
    std::cout << "unexpected output: " << msg << std::endl;
    std::cout << "got: " << got << ", want: " << want << std::endl;
    exit(EXIT_FAILURE);
  }
}

int main(int ac, char **av) {
  (void)ac;
  Config config(av[1]);

  assert(config.vec_server_config_.size(), (size_t)3, "size of server");
  {
    ServerConfig sc = config.vec_server_config_[0];
    assert(sc.vec_location_config_.size(), (size_t)1, "size of location");

    assert(sc.host_, std::string("127.0.0.1"), "host");
    assert(sc.port_, (size_t)80, "port");
    assert(sc.vec_server_names_.size(), (size_t)1, "size of server_name");
    assert(sc.vec_server_names_[0], std::string("localhost"), "server_name");
    assert(sc.client_max_body_size_, (size_t)10240, "client_max_body_size");

    LocationConfig lc = sc.vec_location_config_[0];
    assert(lc.vec_accepted_method_.size(), (size_t)1, "size of http_method");
    assert(lc.vec_accepted_method_[0], std::string("GET"), "http_method");
    assert(lc.proxy_pass_, std::string("http://localhost:8080/"), "proxy_pass");
    assert(lc.root_, std::string("/etc/www"), "root");
    assert(lc.vec_index_.size(), (size_t)2, "size of index");
    assert(lc.vec_index_[0], std::string("index.html"), "index1");
    assert(lc.vec_index_[1], std::string("index.pl"), "index2");
    assert(lc.vec_cgi_file_extension_.size(), (size_t)3,
           "size of add_cgi_handler");
    assert(lc.vec_cgi_file_extension_[0], std::string(".php"),
           "add_cgi_handler1");
    assert(lc.vec_cgi_file_extension_[1], std::string(".cgi"),
           "add_cgi_handler2");
    assert(lc.vec_cgi_file_extension_[2], std::string(".pl"),
           "add_cgi_handler3");
    assert(lc.is_uploadable_, true, "is_uploadable");
    assert(lc.upload_dir_, std::string("/img"), "upload_store");

    std::cout << "test all items passed :)" << std::endl;
  }
  {
    ServerConfig sc = config.vec_server_config_[1];
    assert(sc.vec_location_config_.size(), (size_t)2, "size of location");
    assert(sc.host_, std::string(""), "host");
    assert(sc.port_, (size_t)88, "port");
    assert(sc.vec_server_names_.size(), (size_t)1, "size of server_name");
    assert(sc.vec_server_names_[0], std::string("localhost"), "server_name");
    assert(sc.client_max_body_size_, (size_t)1024, "client_max_body_size");

    LocationConfig lc = sc.vec_location_config_[0];
    assert(lc.vec_accepted_method_.size(), (size_t)3, "size of http_method");
    assert(lc.vec_accepted_method_[0], std::string("GET"), "http_method1");
    assert(lc.vec_accepted_method_[1], std::string("POST"), "http_method2");
    assert(lc.vec_accepted_method_[2], std::string("DELETE"), "http_method3");
    assert(lc.proxy_pass_, std::string(""), "proxy_pass");
    assert(lc.root_, std::string("/etc/www"), "root");
    assert(lc.vec_index_.size(), (size_t)1, "size of index");
    assert(lc.vec_cgi_file_extension_.size(), (size_t)0,
           "size of add_cgi_handler");
    assert(lc.is_uploadable_, false, "is_uploadable");
    assert(lc.upload_dir_, std::string(""), "upload_store");

    lc = sc.vec_location_config_[1];
    assert(lc.vec_accepted_method_.size(), (size_t)3, "size of http_method");
    assert(lc.vec_accepted_method_[0], std::string("GET"), "http_method1");
    assert(lc.vec_accepted_method_[1], std::string("POST"), "http_method2");
    assert(lc.vec_accepted_method_[2], std::string("DELETE"), "http_method3");
    assert(lc.proxy_pass_, std::string(""), "proxy_pass");
    assert(lc.root_, std::string(""), "root");
    assert(lc.vec_index_.size(), (size_t)1, "size of index");
    assert(lc.vec_index_[0], std::string("index.html"), "index1");
    assert(lc.vec_cgi_file_extension_.size(), (size_t)0,
           "size of add_cgi_handler");
    assert(lc.is_uploadable_, false, "is_uploadable");
    assert(lc.upload_dir_, std::string(""), "upload_store");

    std::cout << "test multi location passed :)" << std::endl;
  }
  {
    ServerConfig sc = config.vec_server_config_[2];
    assert(sc.vec_location_config_.size(), (size_t)0, "size of location");
    assert(sc.host_, std::string(""), "host");
    assert(sc.port_, (size_t)80, "port");
    assert(sc.vec_server_names_.size(), (size_t)0, "size of server_name");
    assert(sc.client_max_body_size_, (size_t)1024, "client_max_body_size");

    std::cout << "test no location passed :)" << std::endl;
  }
}
