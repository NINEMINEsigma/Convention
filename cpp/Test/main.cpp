#include "Convention/std_instance.h"
#include <Windows.h>

using namespace std;
using namespace convention_kit;

int main()
{
	instance<vector<int>> vec{ 1,2,3,4 ,5};
	auto m = make_matrix(vec);
	cout << m;
	m = make_matrix(make_view(vec, 0, 4));
	cout << "\n\n" << m;
}