#include "Convention/app_instance.h"
#include <Windows.h>
#include <shlobj.h>

using namespace std;

const std::filesystem::path& instance<platform_indicator, true>::persistent_path() const noexcept
{
	static auto path = []() -> std::filesystem::path
		{
			//auto p = ::getenv("HOME");
			//if (p)return std::filesystem::path(p);
			PWSTR path_tmp;
			auto get_folder_path_ret = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path_tmp);
			/* Error check */
			if (get_folder_path_ret != S_OK) 
			{
				CoTaskMemFree(path_tmp);
				return "./";
			}
			std::filesystem::path path = path_tmp;
			CoTaskMemFree(path_tmp);
			return path;
		}();
	return path;
}

