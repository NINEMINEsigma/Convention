#include "Convention.h"

using namespace std;
using std::filesystem::path;
wchar_t buffer[256] = { 0 };

void add_to_music_list(vector<path>& ml, wstring path_);
auto make_list(instance<config_indicator::tag>& config)
{
	if (
		config.vec().size()<2||
		config.contains("l") + config.contains("s") != 1 ||
		config.contains("h"))
	{
		cout << "player argument" << endl;
		cout << "\tmust specify one and only one" << endl;
		cout << "\t\t-l\tmusic list name like \"musiclist.txt\"" << endl;
		cout << "\t\t-s\tsong [song2[song...]]" << endl;
		cout << "\tthe way the playlist is played" << endl;
		cout << "\t\t-rl\trandom, this mode is default" << endl;
		cout << "\t\t-ll\tlist loops" << endl;
		cout << "\t\t-sl\tsingle loop" << endl;
		cout << "\toptional" << endl;
		cout << "\t\t-ff\targument to ffplay, default is \"-v info -vn -showmode 0\"(-autoexit is setup inside)" << endl;
		cout << "player [-rl/-ll/sl] [-ff:\"...\"] -l \"musiclist.txt\"(or -s song [songs...])";
		exit(0);
	}
	vector<path> result;
	if (config.contains("l"))
	{
		auto file = make_instance<path>(config.string_value("l"));
		for (auto&& path_ : unpack_lines_from_file(file->c_str(), ios::in, buffer, 256))
			add_to_music_list(result, path_);
	}
	else
	{
		for (auto&& i : config.list("s"))
			result.push_back(i);
	}
	return result;
}

int main(int argv, char** argc)
{
	srand(time(0));

	auto config = make_instance<config_indicator::tag>(argv, argc);
	vector<path> musiclist = make_list(config);

	try
	{

		// main loop
		int next_song = config.contains("sl") ? 2 : (config.contains("ll") ? 1 : 0);
		vector<function<size_t(size_t)>> next_song_funcs = {
			// random
			[&musiclist](size_t) {return rand() % musiclist.size(); },
			// list loop
			[&musiclist](size_t index) {return (index + 1) % musiclist.size(); },
			// single loop
			[](size_t index) {return index; }
		};
		auto player = make_instance<ffmpeg_indicator>(
			config.contains("ff") != 0
			? make_string("-autoexit ") + string_indicator::to_string(config.string_value("ff"))
			: make_string("-autoexit -v info -vn -showmode 0")
		);
		size_t index = rand() % musiclist.size(), last_index = 0;
		while (index < musiclist.size())
		{
			int ch = -1;
			SetConsoleTitle(string_indicator::tag(musiclist[index].filename()).c_str());
			player.ffplay(musiclist[index]);
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
				try { cout << "\tprevious(-p)\t\tGo back to the previous song<" << musiclist[last_index].filename(); }
				catch (...) {} cout << ">" << endl;
				try { cout << "\tback(-b)\t\tTo song<" << musiclist[(index - 1) % musiclist.size()].filename(); }
				catch (...) {} cout << ">" << endl;
				try { cout << "\tnext(-n)\t\tTo song<" << musiclist[(index + 1) % musiclist.size()].filename(); }
				catch (...) {} cout << ">" << endl;
				cout << "\t{song name}\t\tenter song name and prefix matching it" << endl;
				string mode;
				cin >> mode;
				if (mode == "previous" || mode == "-p")
				{
					std::swap(index, last_index);
					continue;
				}
				else if (mode == "back" || mode == "-b")
				{
					last_index = index;
					index = (index - 1) % musiclist.size();
				}
				else if (mode == "next" || mode == "-n")
				{
					last_index = index;
					index = (index + 1) % musiclist.size();
				}
				else
				{
					auto iter = std::find_if(
						musiclist.begin(), musiclist.end(),
						[&mode](const path& path)->bool
						{
							try
							{
								return path.filename().string()._Starts_with(mode);
							}
							catch (...)
							{
								return false;
							}
						});
					for (; iter == musiclist.end(); iter = std::find_if(
						musiclist.begin(), musiclist.end(),
						[&mode](const path& path)->bool
						{
							try
							{
								return path.filename().string()._Starts_with(mode);
							}
							catch (...)
							{
								return false;
							}
						}))
					{
						cout << "not found target, input song name:";
						cin >> mode;
					}
					last_index = index;
					index = distance(musiclist.begin(), iter);
				}
			}
			break;
			default:
			{
				last_index = index;
				index = next_song_funcs[next_song](index);
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

void add_to_music_list(vector<path>& ml, wstring path_)
{
	if (path_.size() == 0)return;
	while (path_.back() == L'\n')
		path_.erase(path_.end() - 1);
	if (filesystem::is_directory(path_) == false)
		ml.push_back(path_);
	else for (auto&& file : filesystem::directory_iterator(path_))
		if (filesystem::is_directory(file) == false)
			ml.push_back(file.path());
}
