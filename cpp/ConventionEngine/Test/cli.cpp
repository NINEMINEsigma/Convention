#include <iostream>
#include "Convention.h"
#include "ConventionEngine/Kernel/CoreModule.h"

using namespace std;


int main()
{
	std::filesystem::path p = "xx.xx";
	cout << p.extension() << endl;
}