#ifdef _WINDOWS
#include "Convention/console_instance.h"

using namespace std;
using console = ::instance<console_indicator, true>;

console::instance() noexcept:_Mybase(new console_indicator::tag())
{
	this->get()->hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	this->get()->hBuffer = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	SetConsoleActiveScreenBuffer(this->get()->hBuffer);
	SetConsoleCursorInfo(this->get()->hBuffer, &this->get()->cci);
	SetConsoleMode(this->get()->hBuffer,
		ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING
	);
}
console::~instance()
{
	CloseHandle(this->get()->hBuffer);
}

console& console::set_buffer(_In_ char* data, _Out_opt_ char** erase_)
{
	if (erase_)
		*erase_ = this->get()->data;
	this->get()->data = data;
	return *this;
}

console& console::set_cursor(DWORD size, BOOL visible)
{
	this->get()->cci.bVisible = visible;
	this->get()->cci.dwSize = size;
	SetConsoleCursorInfo(this->get()->hOutput, &this->get()->cci);
	SetConsoleCursorInfo(this->get()->hBuffer, &this->get()->cci);
	return *this;
}

constexpr static size_t temp_buffer_size = 1024;
char static_buffer[temp_buffer_size];

console& console::refresh(size_t size)
{
	COORD coord = { 0,0 };
	if (this->get()->data == nullptr)
	{
		if (size > temp_buffer_size)
			throw std::overflow_error("this console buffer is loss, and current argument<size> is bigger than the global temporary buffer");
		ReadConsoleOutputCharacterA(
			this->get()->hOutput,
			static_buffer,
			size,
			coord,
			&this->get()->bytes
		);
		WriteConsoleOutputCharacterA(
			this->get()->hBuffer,
			static_buffer,
			size,
			coord,
			&this->get()->bytes
		);
	}
	else
	{
		ReadConsoleOutputCharacterA(
			this->get()->hOutput,
			this->get()->data,
			size,
			coord,
			&this->get()->bytes
		);
		WriteConsoleOutputCharacterA(
			this->get()->hBuffer,
			this->get()->data,
			size,
			coord,
			&this->get()->bytes
		);
	}
	SetConsoleCursorPosition(this->get()->hOutput, coord);
	return *this;
}

#endif

