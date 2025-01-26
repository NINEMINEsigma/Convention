#include"Convention.h"

using namespace std;


int main(int argv, char** argc)
{
	cout << instance<platform_indicator>().persistent_path() << endl;
	auto assets = make_instance(instance<platform_indicator>().persistent_path());
	assets.must_exist_path();
	cout << assets.SymbolName() << ": " << assets.ToString();
}