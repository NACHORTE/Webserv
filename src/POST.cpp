#include "HttpMethods.hpp"
#include "utils.hpp"

HttpResponse POST(const HttpRequest & req, const Locations & valid_paths)
{
	(void)req;
	(void)valid_paths;
	HttpResponse ret;
	ret.set_status(200, "POST OK");
	ret.set_body("text/html", "<html><body><h1>POST</h1></body></html>");
	return ret;
}
