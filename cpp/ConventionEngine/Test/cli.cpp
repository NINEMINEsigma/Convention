#include <iostream>
#include "Convention.h"

using namespace std;

class A
{
public:
	void* operator new(size_t size)
	{
		cout << "A" << endl;
		return ::operator new(size);
	}
};

class B :public A
{
public:
};

int main()
{
	A* ptr = new B();
}