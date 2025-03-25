#include "Convention.h"

using namespace std;
using namespace convention_kit;

#pragma region Algorithm

static double delay = 0.001;
static bool is_lowstep = false;
inline void wait()
{
	if (delay == 0)
	{
		cout << "\npause to continue" << endl;
		char a;
		cin.get(a);
	}
	else
	{
#ifdef _WINDOWS
		Sleep(delay * 1000);
#endif // _WINDOWS

	}
}

#define make_algorithm(name, description) std::make_pair(#name, make_descriptive<function<void(void)>>(name,description))
void SelectSort();
void BubbleSort();
void InsertSort();
void InsertSort2();
void CountingSort();

static map<string, descriptive_indicator<function<void(void)>>> algorithms = {
	make_algorithm(SelectSort, "sorting/select"),
	make_algorithm(BubbleSort, "sorting/bubble"),
	make_algorithm(InsertSort, "sorting/insert"),
	make_algorithm(InsertSort2, "sorting/insert-o"),
	make_algorithm(CountingSort, "sorting/count")
};

#pragma endregion

static instance<config_indicator::tag> global_config(0, nullptr);
void config_make_helper(instance<config_indicator::tag>& config)
{
	std::string algorithm_names;
	for (auto&& [key, _] : algorithms)
		algorithm_names += key + ",";
	cout << config.make_manual(
		"global arguments:",
		make_descriptive("-a", Combine("algorithms, foreach or ", algorithm_names).c_str()),
		make_descriptive("-s", "wait seconds and show step by step, if 0 will wait for keyboard input"),
		make_descriptive("-lowstep", "show fewer steps to reduce the time consuming of the output")
	) << endl;
	cout << config.make_manual(
		"sorting arguments:",
		make_descriptive("-arraysize", "numbers size, default 10"),
		make_descriptive("-fillsize", "the higher the value, the greater the horizontal tabulation width"),
		make_descriptive("-upboundary", "number up boundary, default 10")
	) << endl;
}
void config_checker(instance<config_indicator::tag>& config)
{
	SetConsoleTitleA("Visual Algorithm");
	srand(time(0));
	global_config = config;
	if (config.is_contains_helper_command() || config.vec().size() == 1)
	{
		config_make_helper(config);
		exit(0);
	}
	if (config.is_contains_version_command())
	{
		cout << config.version();
		exit(0);
	}
	config("s", delay);
	is_lowstep = config.contains("lowstep");
	for (auto&& a_va : config.list("a"))
	{
		system("cls");
		if (a_va == "foreach")
		{
			for (auto&& algorithm : algorithms)
			{
				SetConsoleTitleA(algorithm.first.c_str());
				system("cls");
				algorithm.second.target();
				Sleep(1000);
			}
		}
		else
		{
			if (algorithms.count(a_va))
			{
				SetConsoleTitleA(a_va.c_str());
				system("cls");
				algorithms[a_va].target();
			}
			else
			{
				cout << a_va << " is not found" << endl;
			}
			cout << "pause to continue" << endl;
			while (platform_indicator::keyboard_input() == -1);
		}
	}
	SetConsoleTitleA("");
	cout << "\n";
	for (auto&& [key, name] : global_config.try_get_histroy)
		cout << Combine("-", key, "[", name, "] ");
	cout << endl;
}
void config_algorithm(instance<config_indicator::tag>& config, function<void(void)> algorithm)
{
	srand(time(0));
	global_config = config;
	if (config.is_contains_helper_command())
	{
		config_make_helper(config);
		exit(0);
	}
	if (config.is_contains_version_command())
	{
		cout << config.version();
		exit(0);
	}
	config("s", delay);
	is_lowstep = config.contains("lowstep");
	system("cls");
	algorithm();
	cout << "\n";
	for (auto&& [key, name] : global_config.try_get_histroy)
		cout << Combine("-", key, "[", name, "] ");
	cout << endl;
}

#pragma region Enumerate Algorithms



#pragma endregion

#pragma region Sorting Algorithms

string draw_single_label(
	string label,
	int fill_size,
	vector<int>& numbers
)
{
	string buffer;
	buffer += " | ";
	buffer += GetColorCodeA(ConsoleColor::None) + GetBackgroundColorCodeA(ConsoleBackgroundColor::None);
	string subs = label.substr(0, fill_size - 1);
	buffer += subs;
	if (label.size() < fill_size)
		buffer += string(std::max<int>(0, fill_size - subs.size() - 1), ' ');
	return buffer;
}
string draw_single_line(
	int i, int fill_size,
	vector<int>& numbers,
	const map<int, ConsoleBackgroundColor>& indexs
)
{
	string buffer;
	//numbers
	buffer += " | ";
	if (indexs.count(i))
		buffer += GetBackgroundColorCodeA(indexs.find(i)->second);
	else
		buffer += GetBackgroundColorCodeA(ConsoleBackgroundColor::White);
	int current = i < numbers.size() ? numbers[i] : 0;
	int max_number = *max_element(numbers.begin(), numbers.end());
	if (max_number < fill_size)
	{
		buffer += string(current, ' ');
		buffer += GetColorCodeA(ConsoleColor::None) + GetBackgroundColorCodeA(ConsoleBackgroundColor::None);
		buffer += string(std::max(0, fill_size - 1 - current), ' ');
	}
	else
	{
		int numberxx = 0;
		for (int xx = current; xx; xx /= 10)
			numberxx++;
		int front_size = std::max(0, current * fill_size / max_number - numberxx - 1);
		if (current != 0)
		{
			buffer += string(front_size, ' ');
			buffer += GetColorCodeA(ConsoleColor::Black) + to_string(current);
		}
		buffer += GetColorCodeA(ConsoleColor::None) + GetBackgroundColorCodeA(ConsoleBackgroundColor::None);
		buffer += string(std::max(0, fill_size - front_size - numberxx - 1), ' ');
	}
	return buffer;
}
void current_sorting_array_numbers(
	vector<int>& numbers,
	const map<int, ConsoleBackgroundColor>& indexs
)
{
	string buffer;
	buffer.reserve(4096);
	size_t fill_size = global_config.try_int_value("fillsize", *max_element(numbers.begin(), numbers.end()) + 3);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ 0,0 });
	for (int i = 0, e = numbers.size(); i < e; i++)
	{
		buffer += draw_single_line(i, fill_size, numbers, indexs) + "\n";
	}
	cout << buffer << endl;
	wait();
}
void current_sorting_array_numbers(
	vector<string> label_list,
	vector<vector<int>*> numbers_list,
	vector<map<int, ConsoleBackgroundColor>> indexs_list
)
{
	string buffer;
	buffer.reserve(4096);
	size_t fill_size = global_config.try_int_value("fillsize", *max_element(numbers_list[0]->begin(), numbers_list[0]->end()) + 3);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ 0,0 });
	for (int i = 0, e = label_list.size(); i != e; i++)
		buffer += draw_single_label(label_list[i], fill_size, *numbers_list[i]);
	buffer += "\n";
	int e = 0;
	for (auto&& ptr : numbers_list)
		e = std::max<int>(e, ptr->size());
	for (int i = 0; i < e; i++)
	{
		//numbers
		for (int j = 0, je = label_list.size(); j != je; j++)
			buffer += draw_single_line(i, fill_size, *numbers_list[j], indexs_list[j]);
		buffer += "\n";
	}
	cout << buffer << endl;
	wait();
}

void SelectSort()
{
	vector<int> numbers;
	int upboundary = global_config.try_int_value("upboundary", 10);
	for (int i = 0, e = global_config.try_int_value("arraysize", 10); i < e; i++)
		numbers.push_back(rand() % upboundary);
	current_sorting_array_numbers(numbers, {});
	for (int i = 0, e = numbers.size(); i < e; i++)
	{
		int min_i = i;
		for (int j = i + 1; j < e; j++)
		{
			if (is_lowstep == false)
				current_sorting_array_numbers(
					numbers,
					{
						make_pair(i, ConsoleBackgroundColor::Green),
						make_pair(min_i, ConsoleBackgroundColor::Red),
						make_pair(j, ConsoleBackgroundColor::Yellow)
					});
			if (numbers[j] < numbers[min_i])
			{
				min_i = j;
				current_sorting_array_numbers(
					numbers,
					{
						make_pair(i, ConsoleBackgroundColor::Green),
						make_pair(min_i, ConsoleBackgroundColor::Red)
					});
			}
		}
		if (is_lowstep == false)
			current_sorting_array_numbers(
				numbers,
				{
					make_pair(i, ConsoleBackgroundColor::Green),
					make_pair(min_i, ConsoleBackgroundColor::Red)
				});
		std::swap(numbers[min_i], numbers[i]);
		current_sorting_array_numbers(
			numbers,
			{
				make_pair(min_i, ConsoleBackgroundColor::Green),
				make_pair(i, ConsoleBackgroundColor::Red),
			});
	}
	current_sorting_array_numbers(numbers, {});
	cout << "\n" <<
		"for (int i = 0, e = numbers.size(); i < e; i++)" <<"\n"<<
		"{" <<"\n"<<
		"	int min_i = i;" <<"\n"<<
		"	for (int j = i + 1; j < e; j++)" <<"\n"<<
		"		if (numbers[j] < numbers[min_i])" <<"\n"<<
		"			min_i = j;" <<"\n"<<
		"	swap(numbers[min_i], numbers[i]);" <<"\n"<<
		"}" << endl;
}
void BubbleSort()
{
	int upboundary = global_config.try_int_value("upboundary", 10);
	vector<int> numbers;
	for (int i = 0, e = global_config.try_int_value("arraysize", 10); i < e; i++)
		numbers.push_back(rand() % upboundary);
	bool flag = true;
	while (flag)
	{
		flag = false;
		for (int i = 1, e = numbers.size(); i < e; ++i)
		{
			if (is_lowstep == false)
				current_sorting_array_numbers(numbers,
					{
						make_pair(i - 1,ConsoleBackgroundColor::Yellow)
					});
			if (numbers[i - 1] > numbers[i])
			{
				flag = true;
				std::swap(numbers[i], numbers[i - 1]);
				current_sorting_array_numbers(numbers,
					{
						make_pair(i - 1,ConsoleBackgroundColor::Green),
						make_pair(i,ConsoleBackgroundColor::Red)
					});
			}
		}
	}
	current_sorting_array_numbers(numbers, {});
	cout << "\n" <<
		"bool flag = true;" << "\n" <<
		"while (flag)" << "\n" <<
		"{" << "\n" <<
		"	flag = false;" << "\n" <<
		"	for (int i = 1, e = numbers.size(); i < e; ++i)" << "\n" <<
		"	{" << "\n" <<
		"		if (numbers[i - 1] > numbers[i])" << "\n" <<
		"		{" << "\n" <<
		"			flag = true;" << "\n" <<
		"			std::swap(numbers[i], numbers[i - 1]);" << "\n" <<
		"		}" << "\n" <<
		"	}" << "\n" <<
		"}" << endl;
}
void InsertSort()
{
	int upboundary = global_config.try_int_value("upboundary", 10);
	vector<int> numbers;
	for (int i = 0, e = global_config.try_int_value("arraysize", 10); i < e; i++)
		numbers.push_back(rand() % upboundary);
	for (int i = 1,e= numbers.size(); i < e; ++i)
	{
		int key = numbers[i];
		int j = i - 1;
		while (j >= 0 && numbers[j] > key)
		{
			numbers[j + 1] = numbers[j];
			if (is_lowstep == false)
				current_sorting_array_numbers(numbers,
					{
						make_pair(i,ConsoleBackgroundColor::Green),
						make_pair(j + 1,ConsoleBackgroundColor::Yellow)
					});
			j--;
		}
		numbers[j + 1] = key;
		current_sorting_array_numbers(numbers,
			{
				make_pair(i,ConsoleBackgroundColor::Green),
				make_pair(j + 1,ConsoleBackgroundColor::Red)
			});
	}
	current_sorting_array_numbers(numbers, {});
	cout << "\n" <<
		"for (int i = 1, e = numbers.size(); i < e; ++i)" << "\n" <<
		"{" << "\n" <<
		"	int key = numbers[i];" << "\n" <<
		"	int j = i - 1;" << "\n" <<
		"	while (j >= 0 && numbers[j] > key)" << "\n" <<
		"	{" << "\n" <<
		"		numbers[j + 1] = numbers[j];" << "\n" <<
		"		j--;" << "\n" <<
		"	}" << "\n" <<
		"	numbers[j + 1] = key;" << "\n" <<
		"}" << endl;
}
void InsertSort2() 
{
	vector<int> numbers;
	int upboundary = global_config.try_int_value("upboundary", 10);
	for (int i = 0, e = global_config.try_int_value("arraysize", 10); i < e; i++)
		numbers.push_back(rand() % upboundary);
	for (int i = 1,e=numbers.size(); i != e; ++i) 
	{
		int key = numbers[i];
		auto index = distance(numbers.begin(), upper_bound(numbers.begin(), numbers.begin() + i, key));
		if (is_lowstep == false)
			current_sorting_array_numbers(numbers,
				{
					make_pair(i,ConsoleBackgroundColor::Green)
				});
		memmove(numbers.data() + index + 1, numbers.data() + index, (i - index) * sizeof(int));
		numbers[index] = key;
		current_sorting_array_numbers(numbers,
			{
				make_pair(i,ConsoleBackgroundColor::Green)
			});
	}
	current_sorting_array_numbers(numbers, {});
	cout << "\n" <<
		"for (int i = 1, e = numbers.size(); i != e; ++i)" << "\n" <<
		"{" << "\n" <<
		"	int key = numbers[i];" << "\n" <<
		"	auto index = distance(numbers.begin(), upper_bound(numbers.begin(), numbers.begin() + i, key));" << "\n" <<
		"	memmove(numbers.data() + index + 1, numbers.data() + index, (i - index) * sizeof(int));" << "\n" <<
		"	numbers[index] = key;" << "\n" <<
		"}" << endl;
}
void CountingSort()
{
	vector<int> numbers;
	int upboundary = global_config.try_int_value("upboundary", 10);
	for (int i = 0, e = global_config.try_int_value("arraysize", 10); i < e; i++)
		numbers.push_back(rand() % upboundary);
	int w = *max_element(numbers.begin(), numbers.end())+1;
	vector<int> cnt(w, 0);
	vector<int> results(numbers.size(), 0);
	current_sorting_array_numbers(numbers, {});
	for (int i = 0, e = numbers.size(); i < e; ++i)
	{
		++cnt[numbers[i]];
		current_sorting_array_numbers(
			{ "numbers" ,"cnt" },
			{ &numbers,&cnt },
			{
				{ make_pair(i,ConsoleBackgroundColor::Green) },
				{ make_pair(numbers[i],ConsoleBackgroundColor::Green) }
			});
	}
	for (int i = 0; i < w-1; ++i)
	{
		if (is_lowstep == false)
			current_sorting_array_numbers(
				{ "numbers","cnt" },
				{ &numbers,&cnt },
			{
				{},
				{
					make_pair(i,ConsoleBackgroundColor::Green),
					make_pair(i + 1,ConsoleBackgroundColor::Red)
				}
			});
		cnt[i + 1] += cnt[i];
		current_sorting_array_numbers(
			{ "numbers","cnt" },
			{ &numbers,&cnt },
			{
				{},
				{
					make_pair(i,ConsoleBackgroundColor::Green),
					make_pair(i + 1,ConsoleBackgroundColor::Red)
				}
			});
	}
	for (int i = numbers.size() - 1; i >= 0; --i)
	{
		if (is_lowstep == false)
			current_sorting_array_numbers(
				{ "numbers" ,"cnt","result" },
				{ &numbers ,&cnt,&results },
			{
				{ make_pair(i,ConsoleBackgroundColor::Green) },
				{ make_pair(numbers[i],ConsoleBackgroundColor::Green)},
				{ make_pair(cnt[numbers[i]],ConsoleBackgroundColor::Green)}
			});
		results[cnt[numbers[i]]---1] = numbers[i];
		current_sorting_array_numbers(
			{ "numbers" ,"cnt","result" },
			{ &numbers ,&cnt,&results },
			{
				{ make_pair(i,ConsoleBackgroundColor::Green) },
				{ make_pair(numbers[i],ConsoleBackgroundColor::Green)},
				{ make_pair(cnt[numbers[i]],ConsoleBackgroundColor::Green)}
			});
	}
	current_sorting_array_numbers(
		{ "numbers" ,"cnt","result" },
		{ &numbers ,&cnt,&results },
		{
			{} ,{}, {}
		});
	cout << "\n" <<
		"int w = *max_element(numbers.begin(), numbers.end())+1;"<<"\n"<<
		"for (int i = 0, e=numbers.size(); i < e; ++i) ++cnt[numbers[i]];" << "\n" <<
		"for (int i = 0; i < w-1; ++i) cnt[i] += cnt[i - 1];" << "\n" <<
		"for (int i = numbers.size() - 1; i > 0; --i) results[cnt[numbers[i]]--] = numbers[i];" << endl;
}


#pragma endregion

