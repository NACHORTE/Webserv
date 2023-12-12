#pragma once
#include <iostream>

class HttpRequestHandler
{
	public:
		HttpRequestHandler(void);
		HttpRequestHandler(const HttpRequestHandler & src);
		~HttpRequestHandler(void);
		HttpRequestHandler & operator=(const HttpRequestHandler & rhs);

		void parsemsg(void);
	protected:
	private:
};

std::ostream &operator<<(std::ostream &os, const HttpRequestHandler &obj);
