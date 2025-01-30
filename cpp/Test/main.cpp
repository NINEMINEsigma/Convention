#include"Convention.h"
#include <Windows.h>


using namespace std;
using namespace internal;

int main() 
{
	do
	{
		string buffer(1024, 0);
		instance<console_indicator> console;
		console.set_buffer(buffer.data(), nullptr);
		cout <<ConsoleBackgroundColor::Blue << console.ToString() << "\n";
		cout << console.SymbolName() << "\n";
		console.refresh(buffer.size());
	} while (false);
	cin.get();
}