#include "Convention.h"

using namespace std;
using namespace ConventionKit;

int main(int argc, char** argv)
{
	instance<config_indicator::tag> config(argc, argv);
	string help = config.make_manual(
		"Web Type",
		make_descriptive("server", ""),
		make_descriptive("client", ""),
		"Work Type",
		make_descriptive("broadcast", ""),
		"Option",
		make_descriptive("port", "default 8888")
	);
	if (config.contains("-h") || config.contains("-help"))
	{

	}
	else if (config.contains("server") && config.contains("broadcast"))
	{
		instance<web_indicator::broadcast::server> server;
		if (config.contains("port"))
			server->PORT = config.int_value("port");
		if (server.init())
		{
			server.start();
			return 0;
		}
	}
	else if (config.contains("client") && config.contains("broadcast"))
	{
		instance<web_indicator::broadcast::client> client;
		if (config.contains("port"))
			client->PORT = config.int_value("port");
		if (client.init())
		{
			client.start();
			return 0;
		}
	}
	cout << help;
}