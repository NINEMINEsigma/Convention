#include "va-internal.h"

int main(int argv, char** argc)
{
	instance<config_indicator::tag> config(argv, argc);
	config_algorithm(config, SelectSort);
}