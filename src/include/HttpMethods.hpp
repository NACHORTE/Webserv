#pragma once
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Server.hpp"

HttpResponse GET(const HttpRequest & req, const Server & serv, const Location & loc);
HttpResponse POST(const HttpRequest & req, const Server & serv, const Location & loc);
HttpResponse DELETE(const HttpRequest & req, const Server & serv, const Location & loc);