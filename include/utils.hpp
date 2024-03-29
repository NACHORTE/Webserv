#pragma once
#include <string>

std::string int_to_string(int n);
std::string readFile(const std::string& filePath, bool binary = false);
std::string trim(const std::string& str);
int ok_config(std::string config_file);
char back(std::string str);
void pop_back(std::string& str);
std::string getExtension(const std::string & filename);
std::string getContentType(const std::string & filename);
bool isBinaryFile(const std::string & filename);
