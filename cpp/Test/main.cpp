#include"Convention.h"
using namespace std;
sync_with_stdio_false(__auto__false);
auto __auto__false1 = [](){cout << platform_indicator::generate_platform_message() << endl; return 0; }();

int main()
{
	instance<int> int_true_ptr(95);
	instance<int, false> int_false_ptr(int_true_ptr);
	cout << int_true_ptr.ToString() << ": " << *int_true_ptr << "\n";
	cout << int_false_ptr.ToString() << ": " << *int_false_ptr << "\n";
	cout << (int_true_ptr + int_false_ptr);
	int_true_ptr += int_false_ptr;
	cout << *int_true_ptr;
}