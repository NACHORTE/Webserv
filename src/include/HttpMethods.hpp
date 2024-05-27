#pragma once
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "LocationContainer.hpp"

HttpResponse GET(const HttpRequest & req, const LocationContainer & valid_paths);
HttpResponse POST(const HttpRequest & req, const LocationContainer & valid_paths);
HttpResponse DELETE(const HttpRequest & req, const LocationContainer & valid_paths);