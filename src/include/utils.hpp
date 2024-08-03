#pragma once
#include <string>
#include <set>
#include <map>

std::string decodeURL(std::string str);
std::string extToMime(const std::string & filename);
std::string mimeToExt(const std::string & mime);
const std::set<std::string> & getAllowedHttpMethods(void);
// String functions
char *strdup(const std::string & str);
std::string intToString(int n);
bool startsWith(const std::string & str, const std::string & prefix);
bool endsWith(const std::string & str, const std::string & suffix);
std::string trim(const std::string& str, const std::string & charset = " \t\n\r\f\v");
char back(std::string str);
void pop_back(std::string& str);
std::map<std::string,std::string> get_params(const std::string & str, char separator = ';', char eq = '=');
std::string removeQuotes(const std::string& str);

// file functions
bool isDir(const std::string & path);
std::string readFile(const std::string& filePath, bool binary = false);
std::string getExtension(const std::string & filename);
bool isBinaryFile(const std::string & filename);
std::string joinPath(const std::string & path1, const std::string & path2);
std::string cleanPath(const std::string & path);
std::string baseName(const std::string & path);
std::string dirName(const std::string & path);