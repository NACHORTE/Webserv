#pragma once
#include <iostream>
#include <vector>

class Location
{
	public:
		// Constructors and destructor
		Location(void);
		Location(const std::string &path); // automatically sets isFile and Filename for folders
		Location(const std::string &path,
			bool isFile,
			const std::string &filename,
			const std::vector<std::string> &allowedMethods);
		Location(const Location & src);
		~Location();

		// Setters and getters
		void setPath(const std::string & path);
		void setIsFile(bool isFile);
		void setFilename(const std::string & filename);
		void setAllowedMethods(const std::vector<std::string> & allowedMethods);
		void addAllowedMethod(const std::string & method);

		const std::string & getPath() const;
		const std::string & getFilename() const;
		const std::string & getFilename(const std::string & path) const; // returns the filename of the path
		const std::vector<std::string> & getAllowedMethods() const;

		// Member functions
		bool isFile() const;
		bool isMethodAllowed(const std::string & method) const;
		bool isPathAllowed(const std::string & path) const;

		// Operator overloads
		Location & operator=(const Location & rhs);
		bool operator==(const Location & rhs) const;
	protected:
	private:
		// allowing a file path allows only exact match of the path
		// allowing a directory path allows all files in the directory
		std::string _path; // directory or file http path
		bool _isFile; // true=file, false=directory
		std::string _Filename; // if it is empty, the http path is the filename
		std::vector<std::string> _allowedMethods; // methods that can access this location

	friend std::ostream &operator<<(std::ostream &os, const Location &obj);
};

std::ostream &operator<<(std::ostream &os, const Location &obj);
