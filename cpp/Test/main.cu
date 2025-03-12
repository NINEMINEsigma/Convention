#include "Convention.h"
#include <Windows.h>
#include <stdio.h>

using namespace std;
using namespace convention_kit;

__global__  void test()
{

}

int main()
{
    auto str = make_instance(string("123456789"));
    cout << str << "\n";
    cout << str.at(-0);
}