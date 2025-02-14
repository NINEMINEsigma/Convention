#include "Convention.h"
#include <Windows.h>

using namespace std;
using namespace convention_kit;

int main()
{
	auto filename = make_instance(wstring(L"_ USAO -"));
	cout << filename.contains(L"USAO");
}