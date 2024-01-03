#pragma once
#include <string>

class config {
	public:
	config(std::string config_file);
	~config();

	int read_config(const std::string& config_file);
	std::string get_config(const std::string& key);
	private:
	int port;
	std::string sv_name;
	std::string host;
	std::string root;
	std::string error_page;
	std::string index;
	int max_body;
};

std::string read_data(std::string line, std::string key);