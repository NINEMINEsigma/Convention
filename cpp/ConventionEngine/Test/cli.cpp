#include <iostream>
#include "Convention.h"

using namespace std;


int main()
{
	map<int, int> mapper;
	mapper[5] = 5;
	mapper[3] = 3;
	for (auto&& [_,value] : mapper)
	{
		cout << value << endl;
	}
}