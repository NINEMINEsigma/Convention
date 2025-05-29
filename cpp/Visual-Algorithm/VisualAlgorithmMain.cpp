#include "VisualAlgorithmInternal.h"
#include "Convention.h"

using namespace std;
using namespace ConventionKit;

sync_with_stdio_false(___auto__);

int main(int argc, char** argv)
{
	instance<config_indicator::tag> config(argc, argv);
	ConfigChecker(config);
}