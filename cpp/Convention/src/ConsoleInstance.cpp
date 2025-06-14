#include "Convention/instance/ConsoleInstance.h"

using namespace std;
namespace Convention
{

	// Get the ANSI escape code for a given ConsoleColor
	std::string GetColorCodeA(ConsoleColor color)
	{
		switch (color)
		{
		case ConsoleColor::Green: return "\033[32m";
		case ConsoleColor::Black: return "\033[30m";
		case ConsoleColor::Blue: return "\033[34m";
		case ConsoleColor::Gray: return "\033[37m";
		case ConsoleColor::Purple: return "\033[35m";
		case ConsoleColor::Red: return "\033[31m";
		case ConsoleColor::White: return "\033[37m";
		case ConsoleColor::Cyan: return "\033[36m";
		case ConsoleColor::Yellow: return "\033[33m";
		case ConsoleColor::None: return "\033[0m";
		default: return "\033[0m";
		}
	}

	// Get the ANSI escape code for a given ConsoleBackgroundColor
	std::string GetBackgroundColorCodeA(ConsoleBackgroundColor color)
	{
		switch (color)
		{
		case ConsoleBackgroundColor::Green: return "\033[42m";
		case ConsoleBackgroundColor::Black: return "\033[40m";
		case ConsoleBackgroundColor::Blue: return "\033[44m";
		case ConsoleBackgroundColor::Gray: return "\033[40m";
		case ConsoleBackgroundColor::Purple: return "\033[45m";
		case ConsoleBackgroundColor::Red: return "\033[41m";
		case ConsoleBackgroundColor::White: return "\033[47m";
		case ConsoleBackgroundColor::Cyan: return "\033[46m";
		case ConsoleBackgroundColor::Yellow: return "\033[43m";
		case ConsoleBackgroundColor::None: return "\033[40m";
		default: return "\033[40m";
		}
	}

	// Get the ANSI escape code for a given ConsoleColor
	std::wstring GetColorCodeW(ConsoleColor color)
	{
		switch (color)
		{
		case ConsoleColor::Green: return L"\033[32m";
		case ConsoleColor::Black: return L"\033[30m";
		case ConsoleColor::Blue: return L"\033[34m";
		case ConsoleColor::Gray: return L"\033[37m";
		case ConsoleColor::Purple: return L"\033[35m";
		case ConsoleColor::Red: return L"\033[31m";
		case ConsoleColor::White: return L"\033[37m";
		case ConsoleColor::Cyan: return L"\033[36m";
		case ConsoleColor::Yellow: return L"\033[33m";
		case ConsoleColor::None: return L"\033[0m";
		default: return L"\033[0m";
		}
	}

	// Get the ANSI escape code for a given ConsoleBackgroundColor
	std::wstring GetBackgroundColorCodeW(ConsoleBackgroundColor color)
	{
		switch (color)
		{
		case ConsoleBackgroundColor::Green: return L"\033[42m";
		case ConsoleBackgroundColor::Black: return L"\033[40m";
		case ConsoleBackgroundColor::Blue: return L"\033[44m";
		case ConsoleBackgroundColor::Gray: return L"\033[40m";
		case ConsoleBackgroundColor::Purple: return L"\033[45m";
		case ConsoleBackgroundColor::Red: return L"\033[41m";
		case ConsoleBackgroundColor::White: return L"\033[47m";
		case ConsoleBackgroundColor::Cyan: return L"\033[46m";
		case ConsoleBackgroundColor::Yellow: return L"\033[43m";
		case ConsoleBackgroundColor::None: return L"\033[40m";
		default: return L"\033[40m";
		}
	}

	// Operator overloading for console text color
	std::ostream& operator<< (std::ostream& os, const ConsoleColor& data)
	{
		std::cout << GetColorCodeA(data);
		return os;
	}

	// Operator overloading for console background color
	std::ostream& operator<< (std::ostream& os, const ConsoleBackgroundColor& data)
	{
		std::cout << GetBackgroundColorCodeA(data);

		return os;
	}

	// Operator overloading for console text color
	std::wostream& operator<< (std::wostream& os, const ConsoleColor& data)
	{
		std::wcout << GetColorCodeW(data);
		return os;
	}

	// Operator overloading for console background color
	std::wostream& operator<< (std::wostream& os, const ConsoleBackgroundColor& data)
	{
		std::wcout << GetBackgroundColorCodeW(data);
		return os;
	}

}