#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <vector>

struct LocationConfig
{
	std::string location_path_;
	std::vector<std::string> vec_accepted_method_;
	std::string proxy_pass_;
	std::string root_;
	bool autoindex_;
	std::vector<std::string> vec_index_;
	std::vector<std::string> vec_cgi_file_extension_;
	bool is_uploadable_;
	std::string upload_dir_;
};

struct ServerConfig
{
	int port_;
	std::string host_;
	std::vector<std::string> server_names_;
	std::string error_page_path_;
	int client_max_body_size_;
	std::vector<LocationConfig*> vec_location_config_;
};


#endif
