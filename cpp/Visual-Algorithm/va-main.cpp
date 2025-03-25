#include "va-internal.h"

sync_with_stdio_false(___auto__);

int main(int argc, char** argv)
{
	instance<config_indicator::tag> config(argc, argv);
	config_checker(config);

}