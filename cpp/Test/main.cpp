#include"Convention.h"

using namespace std;


int main(int argv, char** argc)
{
	cout << instance<platform_indicator>().persistent_path();
}