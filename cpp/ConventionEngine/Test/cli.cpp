#include <iostream>
#include "Convention.h"
#include "ConventionEngine/Kernel/CoreModule.h"

using namespace std;


int main()
{
	InitConventionEngine(1024);
	QuitConventionEngine();
	InitConventionEngine(1024);
	QuitConventionEngine();
}