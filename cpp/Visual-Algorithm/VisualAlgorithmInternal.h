#include "Convention.h"

using namespace std;
using namespace ConventionKit;

#pragma region Algorithm

static double delay = 0.001;
static bool isLowStep = false;
constexpr int CONSTEXPR_UPBOUNDARY = 10;
constexpr int CONSTEXPR_ARRAYSIZE = 10;
inline void Wait()
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

#define MakeAlgorithm(name, description) std::make_pair(#name, MakeDescriptive<function<void(void)>>(name,description))
void SelectSort();
void BubbleSort();
void InsertSort();
void InsertSort2();
void CountingSort();

void GenerateParenthesis();
void TwoWayPrefixAnd();

static map<string, DescriptiveIndicator<function<void(void)>>> algorithms = {
	MakeAlgorithm(SelectSort, "排序/选择"),
	MakeAlgorithm(BubbleSort, "排序/冒泡"),
	MakeAlgorithm(InsertSort, "排序/插入"),
	MakeAlgorithm(InsertSort2, "排序/插入-优化"),
	MakeAlgorithm(CountingSort, "排序/计数"),
	MakeAlgorithm(GenerateParenthesis, "括号/生成括号对"),
	MakeAlgorithm(TwoWayPrefixAnd,"前缀和/两道题")
};

#pragma endregion

static instance<config_indicator::tag> globalConfig(0, nullptr);
void ConfigMakeHelper(instance<config_indicator::tag>& config)
{
	std::string algorithmNames;
	for (auto&& [key, _] : algorithms)
		algorithmNames += key + ",";
	cout << config.make_manual(
		"global arguments:",
		MakeDescriptive("-a", Combine("algorithms, foreach or ", algorithmNames).c_str()),
		MakeDescriptive("-s", "wait seconds and show step by step, if 0 will wait for keyboard input"),
		MakeDescriptive("-lowstep", "show fewer steps to reduce the time consuming of the output")
	) << endl;
	cout << config.make_manual(
		"sorting arguments:",
		MakeDescriptive("-arraysize", Combine("numbers size, default ", CONSTEXPR_ARRAYSIZE).c_str()),
		MakeDescriptive("-fillsize", "the higher the value, the greater the horizontal tabulation width"),
		MakeDescriptive("-upboundary", Combine("number up boundary, default ", CONSTEXPR_UPBOUNDARY).c_str())
	) << endl;
	cout << endl;
	cout << "for alorithms:" << endl;
	for (auto&& [algorithm, descriptive] : algorithms)
	{
		cout << algorithm << ": " << descriptive.description << endl;
	}
}
void ConfigChecker(instance<config_indicator::tag>& config)
{
	SetConsoleTitleA("Visual Algorithm");
	srand(time(0));
	globalConfig = config;
	if (config.is_contains_helper_command() || config.vec().size() == 1)
	{
		ConfigMakeHelper(config);
		exit(0);
	}
	if (config.is_contains_version_command())
	{
		cout << config.version();
		exit(0);
	}
	if constexpr (platform_indicator::is_release)
	{
		config("s", delay);
		isLowStep = config.contains("lowstep");
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
		for (auto&& [key, name] : globalConfig.try_get_histroy)
			cout << Combine("-", key, "[", name, "] ");
		cout << endl;
	}
	else
	{
		string a_va = "TwoWayPrefixAnd";
		isLowStep = true;
		delay = 0.0000000000000001;
		SetConsoleTitleA(a_va.c_str());
		system("cls");
		algorithms[a_va].target();
	}
}
void ConfigAlgorithm(instance<config_indicator::tag>& config, function<void(void)> algorithm)
{
	srand(time(0));
	globalConfig = config;
	if (config.is_contains_helper_command())
	{
		ConfigMakeHelper(config);
		exit(0);
	}
	if (config.is_contains_version_command())
	{
		cout << config.version();
		exit(0);
	}
	config("s", delay);
	isLowStep = config.contains("lowstep");
	system("cls");
	algorithm();
	cout << "\n";
	for (auto&& [key, name] : globalConfig.try_get_histroy)
		cout << Combine("-", key, "[", name, "] ");
	cout << endl;
}

#pragma region Draw

string DrawSingleLabel(
	string label,
	int fillSize,
	vector<int>& numbers
)
{
	string buffer;
	buffer += " | ";
	buffer += GetColorCodeA(ConsoleColor::None) + GetBackgroundColorCodeA(ConsoleBackgroundColor::None);
	string subs = label.substr(0, fillSize - 1);
	buffer += subs;
	if (label.size() < fillSize)
		buffer += string(std::max<int>(0, fillSize - subs.size() - 1), ' ');
	return buffer;
}

string DrawSingleLine(
	int i, int fillSize,
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
	int maxNumber = *max_element(numbers.begin(), numbers.end());
	if (maxNumber < fillSize)
	{
		buffer += string(current, ' ');
		buffer += GetColorCodeA(ConsoleColor::None) + GetBackgroundColorCodeA(ConsoleBackgroundColor::None);
		buffer += string(std::max(0, fillSize - 1 - current), ' ');
	}
	else
	{
		int numberxx = 0;
		for (int xx = current; xx; xx /= 10)
			numberxx++;
		int frontSize = std::max(0, current * fillSize / maxNumber - numberxx - 1);
		if (current != 0)
		{
			buffer += string(frontSize, ' ');
			buffer += GetColorCodeA(ConsoleColor::Black) + to_string(current);
		}
		buffer += GetColorCodeA(ConsoleColor::None) + GetBackgroundColorCodeA(ConsoleBackgroundColor::None);
		buffer += string(std::max(0, fillSize - frontSize - numberxx - 1), ' ');
	}
	return buffer;
}

void CurrentSortingArrayNumbers(
	vector<int>& numbers,
	const map<int, ConsoleBackgroundColor>& indexs
)
{
	string buffer;
	buffer.reserve(4096);
	size_t fillSize = globalConfig.try_int_value("fillsize", *max_element(numbers.begin(), numbers.end()) + 3);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ 0,0 });
	for (int i = 0, e = numbers.size(); i < e; i++)
	{
		buffer += DrawSingleLine(i, fillSize, numbers, indexs) + "\n";
	}
	cout << buffer << endl;
	Wait();
}

void CurrentSortingArrayNumbers(
	vector<string> labelList,
	vector<vector<int>*> numbersList,
	vector<map<int, ConsoleBackgroundColor>> indexsList
)
{
	string buffer;
	buffer.reserve(4096);
	size_t fillSize = globalConfig.try_int_value("fillsize", *max_element(numbersList[0]->begin(), numbersList[0]->end()) + 3);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ 0,0 });
	for (int i = 0, e = labelList.size(); i != e; i++)
		buffer += DrawSingleLabel(labelList[i], fillSize, *numbersList[i]);
	buffer += "\n";
	int e = 0;
	for (auto&& ptr : numbersList)
		e = std::max<int>(e, ptr->size());
	for (int i = 0; i < e; i++)
	{
		//numbers
		for (int j = 0, je = labelList.size(); j != je; j++)
			buffer += DrawSingleLine(i, fillSize, *numbersList[j], indexsList[j]);
		buffer += "\n";
	}
	cout << buffer << endl;
	Wait();
}

string DrawColorfulLine(
	int i, int fillSize,
	vector<int>& numbers,
	const map<int, map<int, ConsoleBackgroundColor>>& indexs
)
{
	string buffer;
	//numbers
	buffer += " | ";
	buffer += GetBackgroundColorCodeA(ConsoleBackgroundColor::White);
	int current = i < numbers.size() ? numbers[i] : 0;
	int maxNumber = *max_element(numbers.begin(), numbers.end());
	if (maxNumber < fillSize)
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
		buffer += string(std::max(0, fillSize - 1 - current), ' ');
	}
	else
	{
		int numberxx = std::log10(current);
		int frontSize = std::max(0, current * fillSize / maxNumber - numberxx - 1);
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
		buffer += string(std::max(0, fillSize - frontSize - numberxx - 1), ' ');
	}
	return buffer;
}

void CurrentColorfulArrayNumbers(
	vector<int>& numbers,
	const map<int, map<int, ConsoleBackgroundColor>>& indexs
)
{
	string buffer;
	buffer.reserve(4096);
	size_t fillSize = globalConfig.try_int_value("fillsize", *max_element(numbers.begin(), numbers.end()) + 3);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ 0,0 });
	for (int i = 0, e = numbers.size(); i < e; i++)
	{
		buffer += DrawColorfulLine(i, fillSize, numbers, indexs) + "\n";
	}
	cout << buffer << endl;
	Wait();
}

void CurrentColorfulArrayNumbers(
	vector<string> labelList,
	vector<vector<int>*> numbersList,
	vector<map<int, map<int, ConsoleBackgroundColor>>> indexsList
)
{
	string buffer;
	buffer.reserve(4096);
	size_t fillSize = globalConfig.try_int_value("fillsize", *max_element(numbersList[0]->begin(), numbersList[0]->end()) + 3);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ 0,0 });
	for (int i = 0, e = labelList.size(); i != e; i++)
		buffer += DrawSingleLabel(labelList[i], fillSize, *numbersList[i]);
	buffer += "\n";
	int e = 0;
	for (auto&& ptr : numbersList)
		e = std::max<int>(e, ptr->size());
	for (int i = 0; i < e; i++)
	{
		//numbers
		for (int j = 0, je = labelList.size(); j != je; j++)
			buffer += DrawColorfulLine(i, fillSize, *numbersList[j], indexsList[j]);
		buffer += "\n";
	}
	cout << buffer << endl;
	Wait();
}

#pragma endregion


#pragma region Enumerate Algorithms



#pragma endregion

#pragma region Sorting Algorithms


void SelectSort()
{
	vector<int> numbers;
	int upboundary = globalConfig.try_int_value("upboundary", CONSTEXPR_UPBOUNDARY);
	for (int i = 0, e = globalConfig.try_int_value("arraysize", CONSTEXPR_ARRAYSIZE); i < e; i++)
		numbers.push_back(rand() % upboundary);
	CurrentSortingArrayNumbers(numbers, {});
	for (int i = 0, e = numbers.size(); i < e; i++)
	{
		int min_i = i;
		for (int j = i + 1; j < e; j++)
		{
			if (isLowStep == false)
				CurrentSortingArrayNumbers(
					numbers,
					{
						make_pair(i, ConsoleBackgroundColor::Green),
						make_pair(min_i, ConsoleBackgroundColor::Red),
						make_pair(j, ConsoleBackgroundColor::Yellow)
					});
			if (numbers[j] < numbers[min_i])
			{
				min_i = j;
				CurrentSortingArrayNumbers(
					numbers,
					{
						make_pair(i, ConsoleBackgroundColor::Green),
						make_pair(min_i, ConsoleBackgroundColor::Red)
					});
			}
		}
		if (isLowStep == false)
			CurrentSortingArrayNumbers(
				numbers,
				{
					make_pair(i, ConsoleBackgroundColor::Green),
					make_pair(min_i, ConsoleBackgroundColor::Red)
				});
		std::swap(numbers[min_i], numbers[i]);
		CurrentSortingArrayNumbers(
			numbers,
			{
				make_pair(min_i, ConsoleBackgroundColor::Green),
				make_pair(i, ConsoleBackgroundColor::Red),
			});
	}
	CurrentSortingArrayNumbers(numbers, {});
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
	int upboundary = globalConfig.try_int_value("upboundary", CONSTEXPR_UPBOUNDARY);
	vector<int> numbers;
	for (int i = 0, e = globalConfig.try_int_value("arraysize", CONSTEXPR_ARRAYSIZE); i < e; i++)
		numbers.push_back(rand() % upboundary);
	bool flag = true;
	while (flag)
	{
		flag = false;
		for (int i = 1, e = numbers.size(); i < e; ++i)
		{
			if (isLowStep == false)
				CurrentSortingArrayNumbers(numbers,
					{
						make_pair(i - 1,ConsoleBackgroundColor::Yellow)
					});
			if (numbers[i - 1] > numbers[i])
			{
				flag = true;
				std::swap(numbers[i], numbers[i - 1]);
				CurrentSortingArrayNumbers(numbers,
					{
						make_pair(i - 1,ConsoleBackgroundColor::Green),
						make_pair(i,ConsoleBackgroundColor::Red)
					});
			}
		}
	}
	CurrentSortingArrayNumbers(numbers, {});
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
	int upboundary = globalConfig.try_int_value("upboundary", CONSTEXPR_UPBOUNDARY);
	vector<int> numbers;
	for (int i = 0, e = globalConfig.try_int_value("arraysize", CONSTEXPR_ARRAYSIZE); i < e; i++)
		numbers.push_back(rand() % upboundary);
	for (int i = 1,e= numbers.size(); i < e; ++i)
	{
		int key = numbers[i];
		int j = i - 1;
		while (j >= 0 && numbers[j] > key)
		{
			numbers[j + 1] = numbers[j];
			if (isLowStep == false)
				CurrentSortingArrayNumbers(numbers,
					{
						make_pair(i,ConsoleBackgroundColor::Green),
						make_pair(j + 1,ConsoleBackgroundColor::Yellow)
					});
			j--;
		}
		numbers[j + 1] = key;
		CurrentSortingArrayNumbers(numbers,
			{
				make_pair(i,ConsoleBackgroundColor::Green),
				make_pair(j + 1,ConsoleBackgroundColor::Red)
			});
	}
	CurrentSortingArrayNumbers(numbers, {});
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
	int upboundary = globalConfig.try_int_value("upboundary", CONSTEXPR_UPBOUNDARY);
	for (int i = 0, e = globalConfig.try_int_value("arraysize", CONSTEXPR_ARRAYSIZE); i < e; i++)
		numbers.push_back(rand() % upboundary);
	for (int i = 1,e=numbers.size(); i != e; ++i)
	{
		int key = numbers[i];
		auto index = distance(numbers.begin(), upper_bound(numbers.begin(), numbers.begin() + i, key));
		if (isLowStep == false)
			CurrentSortingArrayNumbers(numbers,
				{
					make_pair(i,ConsoleBackgroundColor::Green)
				});
		memmove(numbers.data() + index + 1, numbers.data() + index, (i - index) * sizeof(int));
		numbers[index] = key;
		CurrentSortingArrayNumbers(numbers,
			{
				make_pair(i,ConsoleBackgroundColor::Green)
			});
	}
	CurrentSortingArrayNumbers(numbers, {});
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
	int upboundary = globalConfig.try_int_value("upboundary", CONSTEXPR_UPBOUNDARY);
	for (int i = 0, e = globalConfig.try_int_value("arraysize", CONSTEXPR_ARRAYSIZE); i < e; i++)
		numbers.push_back(rand() % upboundary);
	int w = *max_element(numbers.begin(), numbers.end())+1;
	vector<int> cnt(w, 0);
	vector<int> results(numbers.size(), 0);
	CurrentSortingArrayNumbers(numbers, {});
	for (int i = 0, e = numbers.size(); i < e; ++i)
	{
		++cnt[numbers[i]];
		CurrentSortingArrayNumbers(
			{ "numbers" ,"cnt" },
			{ &numbers,&cnt },
			{
				{ make_pair(i,ConsoleBackgroundColor::Green) },
				{ make_pair(numbers[i],ConsoleBackgroundColor::Green) }
			});
	}
	for (int i = 0; i < w-1; ++i)
	{
		if (isLowStep == false)
			CurrentSortingArrayNumbers(
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
		CurrentSortingArrayNumbers(
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
		if (isLowStep == false)
			CurrentSortingArrayNumbers(
				{ "numbers" ,"cnt","result" },
				{ &numbers ,&cnt,&results },
			{
				{ make_pair(i,ConsoleBackgroundColor::Green) },
				{ make_pair(numbers[i],ConsoleBackgroundColor::Green)},
				{ make_pair(cnt[numbers[i]],ConsoleBackgroundColor::Green)}
			});
		results[cnt[numbers[i]]---1] = numbers[i];
		CurrentSortingArrayNumbers(
			{ "numbers" ,"cnt","result" },
			{ &numbers ,&cnt,&results },
			{
				{ make_pair(i,ConsoleBackgroundColor::Green) },
				{ make_pair(numbers[i],ConsoleBackgroundColor::Green)},
				{ make_pair(cnt[numbers[i]],ConsoleBackgroundColor::Green)}
			});
	}
	CurrentSortingArrayNumbers(
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
	if (isLowStep)
	{
		CurrentSortingArrayNumbers(drawbuffer,
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
		CurrentSortingArrayNumbers(drawbuffer,
			{
				make_pair(0, ConsoleBackgroundColor::Green),
				make_pair(1, ConsoleBackgroundColor::Red)
			});
		cout << str << string(100, ' ');
	}
}
void GenerateParenthesis()
{
	int upboundary = globalConfig.try_int_value("upboundary", CONSTEXPR_UPBOUNDARY);
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
	int upboundary = globalConfig.try_int_value("upboundary", CONSTEXPR_UPBOUNDARY);
	for (int i = 0, e = globalConfig.try_int_value("arraysize", CONSTEXPR_ARRAYSIZE); i < e; i++)
	{
		double x = (rand() % upboundary / (double)upboundary);
		height.push_back((x * x) * upboundary);
	}
	int n = height.size();
	vector<int> left(n, 0), right(n, 0);
	vector<int> drawArray(n, 0);
	map<int, map<int, ConsoleBackgroundColor>> drawColors;
	if (isLowStep)
	{
		for (int i = 1; i != n; i++)
		{
			drawColors[i][height[i]] = ConsoleBackgroundColor::Yellow;
			drawArray[i] = height[i];
		}
		drawColors[0][*drawArray.begin() = *left.begin() = *height.begin()] = ConsoleBackgroundColor::Green;
		for (int leftI = 1; leftI != n; leftI++)
		{
			drawColors[leftI][left[leftI] = std::max(left[leftI - 1], height[leftI])] = ConsoleBackgroundColor::Green;
			drawArray[leftI] = std::max(drawArray[leftI], left[leftI]);
			auto lineColor = make_pair(leftI, map<int, ConsoleBackgroundColor>{make_pair(drawArray[leftI], ConsoleBackgroundColor::Yellow)});
			CurrentColorfulArrayNumbers(
				{ "graph","left","right","height" },
				{ &drawArray, &left, &right, &height },
				{ drawColors, {lineColor}, {lineColor}, {lineColor} });
		}
		drawColors[n - 1][*drawArray.rbegin() = *right.rbegin() = *height.rbegin()] = ConsoleBackgroundColor::Red;
		for (int rightI = n - 2; rightI != -1; rightI--)
		{
			drawColors[rightI][right[rightI] = std::max(right[rightI + 1], height[rightI + 1])] = ConsoleBackgroundColor::Red;
			drawArray[rightI] = std::max(drawArray[rightI], right[rightI]);
			auto lineColor = make_pair(rightI, map<int, ConsoleBackgroundColor>{make_pair(drawArray[rightI], ConsoleBackgroundColor::Yellow)});
			CurrentColorfulArrayNumbers(
				{ "graph","left","right","height" },
				{ &drawArray, &left, &right, &height },
				{ drawColors, {lineColor}, {lineColor}, {lineColor} });
		}
		int result = 0;
		Wait(), Wait(), Wait(), Wait(), Wait();
		map<int, map<int, ConsoleBackgroundColor>> drawColors2;
		map<int, map<int, ConsoleBackgroundColor>> waterColor;
		vector<int> height2 = height;
		for (int i = 0; i != n; i++)
		{
			waterColor[i][height[i]] = drawColors2[i][height[i]] = ConsoleBackgroundColor::Yellow;
		}
		for (int i = 0; i != n; i++)
		{
			int cheight = std::min(right[i], left[i]);
			int cur = std::max(0, cheight - height[i]);
			if (cur > 0)
			{
				drawColors[i][cheight] = ConsoleBackgroundColor::Blue;
				drawColors2[i][cheight] = ConsoleBackgroundColor::Blue;
				height2[i] = std::max(height2[i], cheight);
				result += cur;
				waterColor[i][cheight] = ConsoleBackgroundColor::Blue;
				auto lineColor = make_pair(i, map<int, ConsoleBackgroundColor>{make_pair(cheight, ConsoleBackgroundColor::Yellow)});
				CurrentColorfulArrayNumbers(
					{ "graph","left","right","height&water" },
					{ &drawArray, &left, &right, &height2 },
					{ drawColors2, {lineColor}, {lineColor}, {waterColor} });
				Wait(), Wait();
				CurrentColorfulArrayNumbers(
					{ "graph","left","right","height&water" },
					{ &drawArray, &left, &right, &height2 },
					{ drawColors, {lineColor}, {lineColor}, {waterColor} });
				Wait(), Wait();
				CurrentColorfulArrayNumbers(
					{ "graph","left","right","height&water" },
					{ &drawArray, &left, &right, &height2 },
					{ drawColors2, {lineColor}, {lineColor}, {waterColor} });
			}
		}
	}
	else
	{
		drawColors[0][*drawArray.begin() = *left.begin() = *height.begin()] = ConsoleBackgroundColor::Green;
		drawColors[n - 1][*drawArray.rbegin() = *right.rbegin() = *height.rbegin()] = ConsoleBackgroundColor::Red;
		for (int i = 1; i != n; i++)
		{
			int leftI = i, rightI = n - i - 1;
			drawColors[leftI][left[leftI] = std::max(left[leftI - 1], height[leftI])] = ConsoleBackgroundColor::Green;
			drawColors[rightI][right[rightI] = std::max(right[rightI + 1], height[rightI + 1])] = ConsoleBackgroundColor::Red;
			drawColors[leftI][height[leftI]] = ConsoleBackgroundColor::Yellow;
			drawColors[rightI][height[rightI]] = ConsoleBackgroundColor::Yellow;

			drawArray[leftI] = std::max(drawArray[leftI], left[leftI]);
			drawArray[rightI] = std::max(drawArray[rightI], right[rightI]);
			CurrentColorfulArrayNumbers(
				{ "graph","left","right","height" },
				{ &drawArray, &left, &right, &height },
				{ drawColors,{}, {}, {} });
		}
		int result = 0;
		for (int i = 0; i != n; i++)
		{
			int cheight = std::min(right[i], left[i]) - height[i];
			int cur = std::max(0, cheight - height[i]);
			if (cur > 0)
			{
				drawColors[i][cheight] = ConsoleBackgroundColor::Blue;
				result += cur;
				CurrentColorfulArrayNumbers(
					{ "graph","left","right","height" },
					{ &drawArray, &left, &right, &height },
					{ drawColors,{}, {}, {} });
			}
		}
	}
	CurrentColorfulArrayNumbers(
		{ "graph","left","right","height" },
		{ &drawArray, &left, &right, &height },
		{ drawColors,{}, {}, {} });
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


