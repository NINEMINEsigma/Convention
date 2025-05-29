#include "Convention.h"

using namespace std;
using namespace ConventionKit;
using std::filesystem::path;
wchar_t buffer[256] = { 0 };

void AddToMusicList(vector<path>& musicList, wstring path);
vector<path> CreateMusicList(instance<ConfigIndicator::tag>& config);

int main(int argv, char** argc)
{
	srand(time(0));

	auto config = CreateInstance<ConfigIndicator::tag>(argv, argc);
	vector<path> musicList = CreateMusicList(config);

	try
	{
		// main loop
		int nextSong = config.contains("sl") ? 2 : (config.contains("ll") ? 1 : 0);
		vector<function<size_t(size_t)>> nextSongFunctions = {
			// random
			[&musicList](size_t) {return rand() % musicList.size(); },
			// list loop
			[&musicList](size_t index) {return (index + 1) % musicList.size(); },
			// single loop
			[](size_t index) {return index; }
		};
		auto player = CreateInstance<FfmpegIndicator>(
			config.contains("ff") != 0
			? CreateString("-autoexit ") + StringIndicator::to_string(config.string_value("ff"))
			: CreateString("-autoexit -v info -vn -showmode 0")
		);
		size_t index = rand() % musicList.size(), lastIndex = 0;
		while (index < musicList.size())
		{
			int ch = -1;
			SetConsoleTitle(string_indicator::tag(musicList[index].filename()).c_str());
			player.ffplay(musicList[index]);
			player.until(
				[&ch]() {return (ch = platform_indicator::keyboard_input()) < 0; },
				[]() {Sleep(10); }
			);
			player.kill();
			switch (ch)
			{
			case ':':
			{
				system("cls");
				cout << "commandline mode:" << endl;
				try { cout << "\tprevious(-p)\t\tGo back to the previous song<" << musicList[lastIndex].filename(); }
				catch (...) {} cout << ">" << endl;
				try { cout << "\tback(-b)\t\tTo song<" << musicList[(index - 1) % musicList.size()].filename(); }
				catch (...) {} cout << ">" << endl;
				try { cout << "\tnext(-n)\t\tTo song<" << musicList[(index + 1) % musicList.size()].filename(); }
				catch (...) {} cout << ">" << endl;
				cout << "\t{song name}\t\tenter song name and prefix matching it" << endl;
				cout << "\trandom-mode(-rl)\t\tenter song name and prefix matching it" << endl;
				cout << "\tlist-mode(-ll)\t\tenter song name and prefix matching it" << endl;
				cout << "\tsingle-mode(-sl)\t\tenter song name and prefix matching it" << endl;
				while (wcin.peek() == '\n')
					wcin.get();
				wcin.getline(buffer, 256, '\n');
				wstring mode(buffer);
				if (mode == L"previous" || mode == L"-p")
				{
					std::swap(index, lastIndex);
					continue;
				}
				else if (mode == L"back" || mode == L"-b")
				{
					lastIndex = index;
					index = (index - 1) % musicList.size();
				}
				else if (mode == L"next" || mode == L"-n")
				{
					lastIndex = index;
					index = (index + 1) % musicList.size();
				}
				else if (mode == L"random-mode" || mode == L"-rl")
				{
					nextSong = 0;
				}
				else if (mode == L"list-mode" || mode == L"-ll")
				{
					nextSong = 1;
				}
				else if (mode == L"single-mode" || mode == L"-sl")
				{
					nextSong = 2;
				}
				else
				{
					map<int, vector<decltype(musicList.begin())>> editDistances;
					for (auto i = musicList.begin(), e = musicList.end();
						i != e; i++)
					{
						try
						{
							auto filename = make_instance(i->filename().wstring());
							int dis = filename.contains(mode) ? 0 : filename.edit_distance(mode);
							editDistances[dis].push_back(i);
						}
						catch (...) {}
					}
					lastIndex = index;
					size_t size = 0;
					bool stats = true;
					for (auto&& [dis, list] : editDistances)
					{
						if (stats == false)break;
						for (auto&& iter : list)
						{
							cout << "\n";
							try
							{
								cout << "is select[y/others(no)]<rank " << dis << ">: " << iter->filename();
								string ch;
								cin >> ch;
								if (ch.empty() == false && (ch[0] == 'y' || ch[0] == 'Y'))
								{
									stats = false;
									index = distance(musicList.begin(), iter);
									break;
								}
							}
							catch (...)
							{
								continue;
							}
						}
					}
				}
			}
			break;
			default:
			{
				lastIndex = index;
				index = nextSongFunctions[nextSong](index);
			}
			break;
			}
		}
	}
	catch (exception ex)
	{
		cerr << "error was happen: " << ex.what() << endl;
		return 1;
	}
	catch (...)
	{
		cerr << "unknown error was happen" << endl;
		return 1;
	}
	return 0;
}

void AddToMusicList(vector<path>& musicList, wstring path)
{
	if (path.size() == 0)return;
	while (path.back() == L'\n')
		path.erase(path.end() - 1);
	if (filesystem::is_directory(path) == false)
		musicList.push_back(path);
	else for (auto&& file : filesystem::directory_iterator(path))
		if (filesystem::is_directory(file) == false)
			musicList.push_back(file.path());
}

vector<path> CreateMusicList(instance<ConfigIndicator::tag>& config)
{
	vector<path> result;
	if (config.vec().size() == 1)
	{
		auto file = make_instance<path>("musiclist.txt");
		if (file.exist())
		{
			for (auto&& path_ : unpack_lines_from_file(file->c_str(), ios::in, buffer, 256))
				AddToMusicList(result, path_);
			return result;
		}
	}
	else if (config.vec().size() == 2)
	{
		if (filesystem::exists(config.vec()[1].first))
		{
			auto file = make_instance<path>(config.vec()[1].first);
			if (file.get_filename().extension() == ".txt")
			{
				for (auto&& path_ : unpack_lines_from_file(file->c_str(), ios::in, buffer, 256))
					AddToMusicList(result, path_);
				return result;
			}
			else
			{
				return vector<path>{ *file };
			}
		}
	}
	if (config.is_contains_helper_command())
	{
		cout << "player argument\n" << config.make_manual(
			"must specify one and only one",
			make_descriptive("-l", "music list name like \"musiclist.txt\""),
			make_descriptive("-s", "song [song2[song...]]"),
			"the way the playlist is played",
			make_descriptive("-rl", "random, this mode is default"),
			make_descriptive("-ll", "list loops"),
			make_descriptive("-sl", "single loop"),
			"toptional",
			make_descriptive("-ff", "argument to ffplay, default is \"-v info -vn -showmode 0\"(-autoexit is setup inside)")
		);
		exit(0);
	}
	if (config.contains("l"))
	{
		auto file = make_instance<path>(config.string_value("l"));
		for (auto&& path_ : unpack_lines_from_file(file->c_str(), ios::in, buffer, 256))
			AddToMusicList(result, path_);
	}
	else
	{
		for (auto&& i : config.list("s"))
			result.push_back(i);
	}
	return result;
}
