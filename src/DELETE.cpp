#include "HttpMethods.hpp"
#include "utils.hpp"

HttpResponse DELETE(const HttpRequest & req, const Locations & valid_paths)
{
	HttpResponse ret;
	ret.set_status(200, "DELETE OK");
	ret.set_body("text/html", "<html><body><h1>DELETE</h1></body></html>");
	return ret;
}
