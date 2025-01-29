#include"Convention.h"
#include <Windows.h>


using namespace std;
using namespace internal;

int main() 
{
	instance<console_indicator> console;
	cout << console.ToString() << endl;
	cout << console.ToString() << endl;
}