#include "Convention.h"

using namespace std;
using std::filesystem::path;
wchar_t buffer[256] = { 0 };

void add_to_music_list(vector<wstring>& ml, wstring path_)
{
	if (path_.size() == 0)return;
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
	auto file = make_instance<path>(argv == 1 ? "list.txt" : argc[1]);
	if (file.exist() == false)
	{
		cout << argc[0] << " " << *file << "(must be exist)" << endl;
		cin.get();
		return 0;
	}

	srand(time(0));
	vector<wstring> musiclist;

	for (auto&& path_ : istream_line_range(L"list.txt", ios::in, buffer, 256))
		add_to_music_list(musiclist, path_);
	if (musiclist.size() == 0)
	{
		cout << *file << " is empty" << endl;
		return 0;
	}

	// main loop
	for (size_t index = rand() % musiclist.size(); index < musiclist.size(); index = rand() % musiclist.size())
	{
		auto player = make_instance<ffmpeg_indicator>(make_string("-autoexit -v info -vn -showmode 0"));
		player.ffplay(musiclist[index]);
		while (player.is_still_alive() && platform_indicator::keyboard_input(0) == false)Sleep(10);
		player.kill();
	}
}

