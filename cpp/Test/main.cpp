#include"Convention.h"
#include <Windows.h>


using namespace std;
using namespace internal;

int main()
{
	cout << ConsoleBackgroundColor::Blue << floor(10.2) << ConsoleBackgroundColor::None;
	instance<console_indicator> console;
	string buffer(1024, 0);
	console.set_buffer(buffer.data(), nullptr);
	cout << ConsoleBackgroundColor::Red << floor(17.9999) << ConsoleBackgroundColor::None << endl;
	console.refresh(1024);
}