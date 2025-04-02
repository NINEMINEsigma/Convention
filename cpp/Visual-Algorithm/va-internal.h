#include "Convention.h"

using namespace std;
using namespace convention_kit;

#pragma region Algorithm

static double delay = 0.001;
static bool is_lowstep = false;
constexpr int constexpr_upboundary = 10;
constexpr int constexpr_arraysize = 10;
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

void GenerateParenthesis();
void TwoWayPrefixAnd();

static map<string, descriptive_indicator<function<void(void)>>> algorithms = {
	make_algorithm(SelectSort, "sorting/选择"),
	make_algorithm(BubbleSort, "sorting/冒泡"),
	make_algorithm(InsertSort, "sorting/插入"),
	make_algorithm(InsertSort2, "sorting/插入-优化"),
	make_algorithm(CountingSort, "sorting/计数"),
	make_algorithm(GenerateParenthesis, "回溯/生成括号对"),
	make_algorithm(TwoWayPrefixAnd,"前缀和/接雨水")
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
		make_descriptive("-arraysize", Combine("numbers size, default ", constexpr_arraysize).c_str()),
		make_descriptive("-fillsize", "the higher the value, the greater the horizontal tabulation width"),
		make_descriptive("-upboundary", Combine("number up boundary, default ", constexpr_upboundary).c_str())
	) << endl;
}
void config_checker(instance<config_indicator::tag>& config)
{
	SetConsoleTitleA("Visual Algorithm");
	srand(time(0));
	global_config = config;
	if constexpr (platform_indicator::is_release)
	{
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
	else
	{
		string a_va = "TwoWayPrefixAnd";
		is_lowstep = true;
		delay = 0.0000000000000001;
		SetConsoleTitleA(a_va.c_str());
		system("cls");
		algorithms[a_va].target();
	}
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

#pragma region Draw

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

string draw_colorful_line(
	int i, int fill_size,
	vector<int>& numbers,
	const map<int, map<int, ConsoleBackgroundColor>>& indexs
)
{
	string buffer;
	//numbers
	buffer += " | ";
	buffer += GetBackgroundColorCodeA(ConsoleBackgroundColor::White);
	int current = i < numbers.size() ? numbers[i] : 0;
	int max_number = *max_element(numbers.begin(), numbers.end());
	if (max_number < fill_size)
	{
		int pos = 0;
		if(indexs.count(i))
			for (auto&& [e, color] : indexs.find(i)->second)
			{
				if (e > current)
				{
					buffer += GetBackgroundColorCodeA(color) + string(current - pos, ' ');
					pos = current;
					break;
				}
				buffer += GetBackgroundColorCodeA(color) + string(e - pos, ' ');
				pos = e;
			}
		buffer += GetBackgroundColorCodeA(ConsoleBackgroundColor::White);
		buffer += string(std::max(0, current - pos), ' ');
		buffer += GetColorCodeA(ConsoleColor::None) + GetBackgroundColorCodeA(ConsoleBackgroundColor::None);
		buffer += string(std::max(0, fill_size - 1 - current), ' ');
	}
	else
	{
		int numberxx = std::log10(current);
		int front_size = std::max(0, current * fill_size / max_number - numberxx - 1);
		if (current != 0)
		{
			int pos = 0;
			if(indexs.count(i))
				for (auto&& tup : indexs.find(i)->second)
				{
					int e = std::log10(get<0>(tup));
					auto&& color = get<1>(tup);
					if (e > numberxx)
					{
						buffer += GetBackgroundColorCodeA(color) + string(numberxx - pos, ' ');
						pos = numberxx;
						break;
					}
					buffer += GetBackgroundColorCodeA(color) + string(e - pos, ' ');
					pos = e;
				}
			buffer += GetBackgroundColorCodeA(ConsoleBackgroundColor::White);
			buffer += string(std::max(0, current - pos), ' ');
			buffer += GetColorCodeA(ConsoleColor::Black) + to_string(current);
		}
		buffer += GetColorCodeA(ConsoleColor::None) + GetBackgroundColorCodeA(ConsoleBackgroundColor::None);
		buffer += string(std::max(0, fill_size - front_size - numberxx - 1), ' ');
	}
	return buffer;
}
void current_colorful_array_numbers(
	vector<int>& numbers,
	const map<int, map<int, ConsoleBackgroundColor>>& indexs
)
{
	string buffer;
	buffer.reserve(4096);
	size_t fill_size = global_config.try_int_value("fillsize", *max_element(numbers.begin(), numbers.end()) + 3);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ 0,0 });
	for (int i = 0, e = numbers.size(); i < e; i++)
	{
		buffer += draw_colorful_line(i, fill_size, numbers, indexs) + "\n";
	}
	cout << buffer << endl;
	wait();
}
void current_colorful_array_numbers(
	vector<string> label_list,
	vector<vector<int>*> numbers_list,
	vector<map<int, map<int, ConsoleBackgroundColor>>> indexs_list
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
			buffer += draw_colorful_line(i, fill_size, *numbers_list[j], indexs_list[j]);
		buffer += "\n";
	}
	cout << buffer << endl;
	wait();
}

#pragma endregion


#pragma region Enumerate Algorithms



#pragma endregion

#pragma region Sorting Algorithms


void SelectSort()
{
	vector<int> numbers;
	int upboundary = global_config.try_int_value("upboundary", constexpr_upboundary);
	for (int i = 0, e = global_config.try_int_value("arraysize", constexpr_arraysize); i < e; i++)
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
	int upboundary = global_config.try_int_value("upboundary", constexpr_upboundary);
	vector<int> numbers;
	for (int i = 0, e = global_config.try_int_value("arraysize", constexpr_arraysize); i < e; i++)
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
	int upboundary = global_config.try_int_value("upboundary", constexpr_upboundary);
	vector<int> numbers;
	for (int i = 0, e = global_config.try_int_value("arraysize", constexpr_arraysize); i < e; i++)
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
	int upboundary = global_config.try_int_value("upboundary", constexpr_upboundary);
	for (int i = 0, e = global_config.try_int_value("arraysize", constexpr_arraysize); i < e; i++)
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
	int upboundary = global_config.try_int_value("upboundary", constexpr_upboundary);
	for (int i = 0, e = global_config.try_int_value("arraysize", constexpr_arraysize); i < e; i++)
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

#pragma region DFS Algorithms

void __generateParenthesis(int left, int right, int depth, string str, vector<int>& drawbuffer)
{
	drawbuffer[0] = left;
	drawbuffer[1] = right;
	drawbuffer[2] = depth;
	if (is_lowstep)
	{
		current_sorting_array_numbers(drawbuffer,
			{
				make_pair(0, ConsoleBackgroundColor::Green),
				make_pair(1, ConsoleBackgroundColor::Red)
			});
		cout << str << string(100, ' ');
	}
	if (left > 0)
		__generateParenthesis(left - 1, right + 1, depth - 1, str + '(', drawbuffer);
	if (right > 0)
		__generateParenthesis(left, right - 1, depth - 1, str + ')', drawbuffer);
	if (left + right == 0)
	{
		current_sorting_array_numbers(drawbuffer,
			{
				make_pair(0, ConsoleBackgroundColor::Green),
				make_pair(1, ConsoleBackgroundColor::Red)
			});
		cout << str << string(100, ' ');
	}
}
void GenerateParenthesis()
{
	int upboundary = global_config.try_int_value("upboundary", constexpr_upboundary);
	if (upboundary > 8)
	{
		upboundary = 8;
	}
	vector<int> drawbuffer = { 0, 0 ,0 };
	__generateParenthesis(upboundary, 0, upboundary * 2, "", drawbuffer);
	cout << "\n" <<
		"if (left > 0)" << "\n" <<
		"	dfs(left - 1, right + 1, str + '(', results);" << "\n" <<
		"if (right > 0)" << "\n" <<
		"	dfs(left, right - 1, str + ')', results);" << "\n" <<
		"if (left + right == 0) result.push_back(str);" << endl;
}

#pragma endregion

#pragma region PrefixAnd

void TwoWayPrefixAnd()
{
	vector<int> height;
	int upboundary = global_config.try_int_value("upboundary", constexpr_upboundary);
	for (int i = 0, e = global_config.try_int_value("arraysize", constexpr_arraysize); i < e; i++)
	{
		double x = (rand() % upboundary / (double)upboundary);
		height.push_back((x * x) * upboundary);
	}
	int n = height.size();
	vector<int> left(n, 0), right(n, 0);
	vector<int> draw_array(n, 0);
	map<int, map<int, ConsoleBackgroundColor>> draw_colors;
	if (is_lowstep)
	{
		for (int i = 1; i != n; i++)
		{
			draw_colors[i][height[i]] = ConsoleBackgroundColor::Yellow;
			draw_array[i] = height[i];
		}
		draw_colors[0][*draw_array.begin() = *left.begin() = *height.begin()] = ConsoleBackgroundColor::Green;
		for (int left_i = 1; left_i != n; left_i++)
		{
			draw_colors[left_i][left[left_i] = std::max(left[left_i - 1], height[left_i])] = ConsoleBackgroundColor::Green;
			draw_array[left_i] = std::max(draw_array[left_i], left[left_i]);
			auto linecolor = make_pair(left_i, map<int, ConsoleBackgroundColor>{make_pair(draw_array[left_i], ConsoleBackgroundColor::Yellow)});
			current_colorful_array_numbers(
				{ "graph","left","right","height" },
				{ &draw_array, &left, &right, &height },
				{ draw_colors, {linecolor}, {linecolor}, {linecolor} });
		}
		draw_colors[n - 1][*draw_array.rbegin() = *right.rbegin() = *height.rbegin()] = ConsoleBackgroundColor::Red;
		for (int right_i = n - 2; right_i != -1; right_i--)
		{
			draw_colors[right_i][right[right_i] = std::max(right[right_i + 1], height[right_i + 1])] = ConsoleBackgroundColor::Red;
			draw_array[right_i] = std::max(draw_array[right_i], right[right_i]);
			auto linecolor = make_pair(right_i, map<int, ConsoleBackgroundColor>{make_pair(draw_array[right_i], ConsoleBackgroundColor::Yellow)});
			current_colorful_array_numbers(
				{ "graph","left","right","height" },
				{ &draw_array, &left, &right, &height },
				{ draw_colors, {linecolor}, {linecolor}, {linecolor} });
		}
		int result = 0;
		wait(), wait(), wait(), wait(), wait();
		map<int, map<int, ConsoleBackgroundColor>> draw_colors2;
		map<int, map<int, ConsoleBackgroundColor>> water_color;
		vector<int> height2 = height;
		for (int i = 0; i != n; i++)
		{
			water_color[i][height[i]] = draw_colors2[i][height[i]] = ConsoleBackgroundColor::Yellow;
		}
		for (int i = 0; i != n; i++)
		{
			int cheight = std::min(right[i], left[i]);
			int cur = std::max(0, cheight - height[i]);
			if (cur > 0)
			{
				draw_colors[i][cheight] = ConsoleBackgroundColor::Blue;
				draw_colors2[i][cheight] = ConsoleBackgroundColor::Blue;
				height2[i] = std::max(height2[i], cheight);
				result += cur;
				water_color[i][cheight] = ConsoleBackgroundColor::Blue;
				auto linecolor = make_pair(i, map<int, ConsoleBackgroundColor>{make_pair(cheight, ConsoleBackgroundColor::Yellow)});
				current_colorful_array_numbers(
					{ "graph","left","right","height&water" },
					{ &draw_array, &left, &right, &height2 },
					{ draw_colors2, {linecolor}, {linecolor}, {water_color} });
				wait(), wait();
				current_colorful_array_numbers(
					{ "graph","left","right","height&water" },
					{ &draw_array, &left, &right, &height2 },
					{ draw_colors, {linecolor}, {linecolor}, {water_color} });
				wait(), wait();
				current_colorful_array_numbers(
					{ "graph","left","right","height&water" },
					{ &draw_array, &left, &right, &height2 },
					{ draw_colors2, {linecolor}, {linecolor}, {water_color} });
			}
		}
	}
	else
	{
		draw_colors[0][*draw_array.begin() = *left.begin() = *height.begin()] = ConsoleBackgroundColor::Green;
		draw_colors[n - 1][*draw_array.rbegin() = *right.rbegin() = *height.rbegin()] = ConsoleBackgroundColor::Red;
		for (int i = 1; i != n; i++)
		{
			int left_i = i, right_i = n - i - 1;
			draw_colors[left_i][left[left_i] = std::max(left[left_i - 1], height[left_i])] = ConsoleBackgroundColor::Green;
			draw_colors[right_i][right[right_i] = std::max(right[right_i + 1], height[right_i + 1])] = ConsoleBackgroundColor::Red;
			draw_colors[left_i][height[left_i]] = ConsoleBackgroundColor::Yellow;
			draw_colors[right_i][height[right_i]] = ConsoleBackgroundColor::Yellow;

			draw_array[left_i] = std::max(draw_array[left_i], left[left_i]);
			draw_array[right_i] = std::max(draw_array[right_i], right[right_i]);
			current_colorful_array_numbers(
				{ "graph","left","right","height" },
				{ &draw_array, &left, &right, &height },
				{ draw_colors,{}, {}, {} });
		}
		int result = 0;
		for (int i = 0; i != n; i++)
		{
			int cheight = std::min(right[i], left[i]) - height[i];
			int cur = std::max(0, cheight - height[i]);
			if (cur > 0)
			{
				draw_colors[i][cheight] = ConsoleBackgroundColor::Blue;
				result += cur;
				current_colorful_array_numbers(
					{ "graph","left","right","height" },
					{ &draw_array, &left, &right, &height },
					{ draw_colors,{}, {}, {} });
			}
		}
	}
	current_colorful_array_numbers(
		{ "graph","left","right","height" },
		{ &draw_array, &left, &right, &height },
		{ draw_colors,{}, {}, {} });
	cout << "\n" <<
		"int n = height.size();" << "\n" <<
		"vector<int> left(n, 0), right(n, 0);" << "\n" <<
		"*left.begin() = *height.begin();" << "\n" <<
		"for (int i = 1; i != n; i++)" << "\n" <<
		"	left[i] = std::max(left[i - 1], height[i]);" << "\n" <<
		"*right.rbegin() = *height.rbegin();" << "\n" <<
		"for (int i = n - 2; i != -1; i--)" << "\n" <<
		"	right[i] = std::max(right[i + 1], height[i]);" << "\n" <<
		"int result = 0;" << "\n" <<
		"for (int i = 0; i != n; i++)" << "\n" <<
		"	result += std::max(0, std::min(right[i], left[i]) - height[i]);" << endl;
}

#pragma endregion


