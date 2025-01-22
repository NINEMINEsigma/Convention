#include"Convention.h"
using namespace std;
sync_with_stdio_false(_);
int main()
{
	cout << platform_indicator::generate_platform_message() << endl;
	char* buffer = no_warning_6387((char*)malloc(50 * sizeof(char)));
	memset(buffer, 0, 50 * sizeof(char));
	any_class* te = new(&buffer[25]) any_class();
	any_class* fe = find_any_class_ptr(buffer, 50);
	cout << te << "\n" << fe << "\n\n";
	void* fev = find_target_flag_class_ptr(
		buffer, 
		50*sizeof(char),
		&any_class_head_flag,
		sizeof(any_class_head_flag),
		sizeof(size_t));
	cout << te << "\n" << fev << endl;
}