#pragma once
#include <string>
#include <map>

std::string intToString(int n);
std::string readFile(const std::string& filePath, bool binary = false);
std::string trim(const std::string& str, const std::string & charset = " \t\n\r\f\v");
char back(std::string str);
void pop_back(std::string& str);
std::string getExtension(const std::string & filename);
std::string extToMime(const std::string & filename);
std::string mimeToExt(const std::string & mime);
bool isBinaryFile(const std::string & filename);
std::map<std::string,std::string> get_params(const std::string & str, char separator = ';', char eq = '=');
std::string removeQuotes(const std::string& str);
std::string decodeURL(std::string str);
char *strdup(const std::string & str);
std::string joinPath(const std::string & path1, const std::string & path2);
std::string cleanPath(const std::string & path);
bool isDir(const std::string & path);
bool startsWith(const std::string & str, const std::string & prefix);
bool endsWith(const std::string & str, const std::string & suffix);
