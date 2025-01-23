#include"Convention.h"
using namespace std;
auto __auto__false1 = [](){cout << platform_indicator::generate_platform_message() << endl; return 0; }();

int main()
{
	instance<filesystem::path> file(L"xx.txt");
	wstring a;
	while (wcin >> a)
		file.get_wstream(ios::out | ios::app) << a;
}