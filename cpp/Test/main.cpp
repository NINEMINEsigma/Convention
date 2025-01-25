#include"Convention.h"

using namespace std;


int main(int argv, char** argc)
{
	for (auto i : filesystem::directory_iterator("."))
		cout << i.path().string() << endl;
}