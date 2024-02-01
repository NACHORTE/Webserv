#pragma once
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Locations.hpp"

HttpResponse GET(const HttpRequest & req, const Locations & valid_paths);
HttpResponse POST(const HttpRequest & req, const Locations & valid_paths);
HttpResponse DELETE(const HttpRequest & req, const Locations & valid_paths);