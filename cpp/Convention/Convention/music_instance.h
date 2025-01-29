#ifndef __FILE_CONVENTION_MUSIC_INSTANCE
#define __FILE_CONVENTION_MUSIC_INSTANCE

#include "Convention/Interface.h"


#if _WINDOWS

#ifndef __NOT_REF_MCI_MUSIC
#include <windows.h>
#include <mmsystem.h>
#include <Digitalv.h>
#pragma comment(lib, "winmm.lib")
struct mci_music_indicator 
{
	using tag = MCIDEVICEID;
	static constexpr bool value = true;
};
template<>
class instance<mci_music_indicator, true> :public instance<mci_music_indicator::tag, false>
{
private:
	using _Mybase = instance<mci_music_indicator::tag, false>;
	void nDeviceID(const mci_music_indicator::tag& id) noexcept
	{
		**this = id;
	}
public:
	bool is_succeed = true;

	mci_music_indicator::tag nDeviceID() const noexcept
	{
		return **this;
	}

	instance()noexcept :_Mybase(new mci_music_indicator::tag(-1)) {}
	instance(nullptr_t)noexcept :_Mybase(nullptr) {}
	explicit instance(mci_music_indicator::tag* ptr)noexcept :_Mybase(ptr) {}
	explicit instance(mci_music_indicator::tag tag)noexcept :_Mybase(new mci_music_indicator::tag(tag)) {}
	template<typename... _Args>
	instance(_Args&&... args) : _Mybase(std::forward<_Args>(args)...) {}
	virtual ~instance()
	{
		if (this->nDeviceID() != -1)
			this->close();
	}
	bool open(const string_indicator::tag& music) noexcept
	{
		MCI_OPEN_PARMS mciOP;

		mciOP.lpstrDeviceType = nullptr;
		mciOP.lpstrElementName = music.c_str();
		this->is_succeed = 0 == mciSendCommand(
			0, MCI_OPEN,
			MCI_OPEN_ELEMENT | MCI_WAIT | MCI_OPEN_SHAREABLE, (DWORD_PTR)(static_cast<LPVOID>(&mciOP)));

		if (this->is_succeed)
		{
			this->nDeviceID(mciOP.wDeviceID);
		}
		return this->is_succeed;
	}
	bool play()noexcept
	{
		MCI_PLAY_PARMS mciPP{};

		return this->is_succeed = 0 == mciSendCommand(
			this->nDeviceID(), MCI_PLAY,
			MCI_NOTIFY, (DWORD_PTR)(static_cast<LPVOID>(&mciPP)));
	}
	bool pause()noexcept
	{
		MCI_GENERIC_PARMS mciGP{};

		return this->is_succeed = 0 == mciSendCommand(
			this->nDeviceID(), MCI_PAUSE,
			MCI_NOTIFY | MCI_WAIT, (DWORD_PTR)(static_cast<LPVOID>(&mciGP)));
	}
	bool stop()noexcept
	{
		MCI_SEEK_PARMS mciSP{};

		return this->is_succeed = 0 == mciSendCommand(
			this->nDeviceID(), MCI_SEEK,
			MCI_WAIT | MCI_NOTIFY | MCI_SEEK_TO_START, (DWORD_PTR)(static_cast<LPVOID>(&mciSP)));
	}
	bool close()noexcept
	{
		MCI_GENERIC_PARMS mciGP{};

		if (this->is_succeed = 0 == mciSendCommand(
			this->nDeviceID(), MCI_CLOSE,
			MCI_NOTIFY | MCI_WAIT, (DWORD_PTR)(static_cast<LPVOID>(&mciGP))))
			this->nDeviceID(-1);
		return this->is_succeed;
	}
	size_t get_current_time()noexcept
	{
		MCI_STATUS_PARMS mciSP{};

		mciSP.dwItem = MCI_STATUS_POSITION;
		this->is_succeed = 0 == mciSendCommand(
			this->nDeviceID(), MCI_STATUS,
			MCI_STATUS_ITEM, (DWORD_PTR)(static_cast<LPVOID>(&mciSP)));
		return static_cast<DWORD>(mciSP.dwReturn);
	}
	size_t get_end_time()noexcept
	{
		MCI_STATUS_PARMS mciSP{};

		mciSP.dwItem = MCI_STATUS_LENGTH;
		this->is_succeed = 0 == mciSendCommand(
			this->nDeviceID(), MCI_STATUS,
			MCI_WAIT | MCI_STATUS_ITEM, (DWORD_PTR)(static_cast<LPVOID>(&mciSP)));
		return static_cast<DWORD>(mciSP.dwReturn);
	}
	bool set_volume(double volume, size_t max_volume = 1000)noexcept
	{
		size_t nVolumeValue = static_cast<size_t>(Clamp01(volume) * max_volume);

		MCI_DGV_SETAUDIO_PARMS mciDSP;
		mciDSP.dwItem = MCI_DGV_SETAUDIO_VOLUME;
		mciDSP.dwValue = static_cast<DWORD>(nVolumeValue);
		return this->is_succeed = 0 == mciSendCommand(
			this->nDeviceID(), MCI_SETAUDIO,
			MCI_DGV_SETAUDIO_VALUE | MCI_DGV_SETAUDIO_ITEM, (DWORD_PTR)(static_cast<LPVOID>(&mciDSP)));
	}
	bool set_start_time(size_t start_time)noexcept
	{
		DWORD end_time = this->get_end_time();

		if (start_time > end_time)
			return (this->is_succeed = false);

		MCI_PLAY_PARMS mciPlay{};
		mciPlay.dwFrom = static_cast<DWORD>(start_time);
		mciPlay.dwTo = static_cast<DWORD>(end_time);
		return this->is_succeed = 0 == mciSendCommand(
			this->nDeviceID(), MCI_PLAY,
			MCI_TO | MCI_FROM, (DWORD_PTR)(static_cast<LPVOID>(&mciPlay)));
	}
};
#endif //__NOT_REF_MCI_MUSIC

#endif _WINDOWS

#endif // !__FILE_CONVENTION_MUSIC_INSTANCE
