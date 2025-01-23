#include "Convention.h"

using namespace std;
using std::filesystem::path;

bool help(int argv, char** argc)
{
	if (argv > 2 || (argv == 2 && strcmp(argc[1], "-h") == 0))
	{
		cout << argc[0] << " musiclist.txt" << endl;
		return true;
	}
	return false;
}

void add_to_music_list(vector<wstring>& ml, wstring path_)
{
	while (path_.back() == L'\n')
		path_.erase(path_.end() - 1);
	if (filesystem::is_directory(path_) == false)
		ml.push_back(path_);
	else for (auto&& file : filesystem::directory_iterator(path_))
		if (filesystem::is_directory(file) == false)
			ml.push_back(file.path().wstring());
}

int main(int argv, char** argc)
{
	if (help(argv, argc))return 0;
	srand(time(0));
	vector<wstring> musiclist;
	instance<path> musiclist_file(argv == 1 ? "musiclist.txt" : argc[1]);
	wchar_t wbuffer[256] = { 0 };
	if (musiclist_file.exist() == false && musiclist_file.is_dir() == false)musiclist_file.must_exist_path();

	// open music list file
	auto musiclist_fs = musiclist_file.get_wstream(ios::in);
	// read music list
	while (musiclist_fs.getline(wbuffer, 256))add_to_music_list(musiclist, wbuffer);

	// main loop
	wcout << L"enter any key to play next audio, enter 'quit' to quit." << endl;
	for (size_t index = rand() % musiclist.size(); index < musiclist.size(); index = rand() % musiclist.size())
	{
		wcout << L"\rcurrent play : " << musiclist[index] <<"\t\t\t\t\t\t\t" << endl;
		if (wcout.bad())
		{
			wcout.setstate(ios::beg);
			wcout.clear();
		}
		instance<ffmpeg_indicator> player;
		player.when_destructor_kill_but_not_wait = true;
		player.play_quiet(musiclist[index]);
		while (player.is_still_alive() && platform_indicator::keyboard_input(0) == false)Sleep(10);
	}
}

