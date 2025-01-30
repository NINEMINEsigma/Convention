#ifndef __FILE_CONVENTION_CONSOLE_INSTANCE
#define __FILE_CONVENTION_CONSOLE_INSTANCE

#include "Convention/Interface.h"
#ifdef _WINDOWS
#include "Windows.h"
#endif

struct console_indicator
{
#ifdef _WINDOWS
	struct tag
	{
		HANDLE hOutput = nullptr;
		HANDLE hBuffer = nullptr;
		CONSOLE_CURSOR_INFO cci = { DWORD(0),BOOL(FALSE) };
		DWORD bytes = 0;
		char* data = nullptr;
	};
#else
	using tag = std::ostream;
#endif // _WINDOWS
	static constexpr bool value = true;
};
template<>
class instance<console_indicator, true> :public instance<console_indicator::tag, false>
{
	using _Mybase = instance<console_indicator::tag, false>;
public:
	instance() noexcept;
	virtual ~instance();
	instance& set_buffer(_In_ char* data, _Out_opt_ char** erase_);

#ifdef _WINDOWS
	instance& set_cursor(DWORD size = 0, BOOL visible = TRUE);
#endif // _WINDOWS

	instance& refresh(size_t size);
};

enum class ascii_code
{
	// NULL
	NUL=0,
	// Start Of Heading
	SOH=1, StartOfHeading=1,
	// Start of Text
	STX=2, StartOfText=2,
	// End Of Text
	ETX=3, EndOfText=3,
	// End Of Transmission
	EOT=4, EndOfTransmission=4,
	// Enquiry
	ENQ=5, Enquiry=5,
	// Acknowledge
	ACK=6, Acknowledge=6,
	// Bell
	BEL=7, Bell=7,
	// Backspace
	BS=8, Backspace=8,
	// Horizontal Tab
	HT=9, HorizontalTab=9,
	// Line Feed/New Line
	LF=10, NL=10, LineFeed=10, NewLine=10,
	// Vertical Tab
	VT=11, VerticalTab=11,
	// Form Feed/New Page
	FF=12, NP=12, FormFeed=12, NewPage=12,
	// Carriage Return
	CR=13, CarriageReturn=13,
	// Shift Out
	SO=14, ShiftOut=14,
	// Shift In
	SI=15, ShiftIn=15,
	// Data Link Escape
	DLE=16, DataLinkEscape=16,
	// Device Control 1/Transmission On
	DC1=17, DeviceControl1=17, TransmissionOn=17,
	// Device Control 2
	DC2=18, DeviceControl2=18,
	// Device Control 3/Transmission Off
	DC3=19, XOFF=19, DeviceControl3=19, TransmissionOff=19,
	// Device Control 4
	DC4=20, DeviceControl4=20,
	// Negative Acknowledge
	NAK=21, NegativeAcknowledge=21,
	// Synchronous Idle
	SYN=22, SynchronousIdle=22,
	// End of Transmission Block
	ETB=23, EndOfTransmissionBlock=23,
	// Cancel
	CAN=24, Cancel=24,
	// End of Medium
	EM=25, EndOfMedium=25,
	// Substitute
	SUB=26, Substitute=26,
	// Escape
	ESC=27, Escape=27,
	// File Separator
	FS=28, FileSeparator=28,
	// Group Separator
	GS=29, GroupSeparator=29,
	// Record Separator
	RS=30, RecordSeparator=30,
	// Unit Separator
	US=31, UnitSeparator=31,
	// Delete
	DEL=127, Delete=127
};

// Enum class for console text color
enum class ConsoleColor
{
	Green, Red, Blue, White, Black, Yellow, Purple, Gray, Cyan, None,
	GreenIntensity, RedIntensity, BlueIntensity, WhiteIntensity, BlackIntensity, YellowIntensity, PurpleIntensity, GrayIntensity, CyanIntensity
};

// Enum class for console background color
enum class ConsoleBackgroundColor
{
	Green, Red, Blue, White, Black, Yellow, Purple, Gray, Cyan, None
};

extern std::ostream& operator<<    (std::ostream& os, const ConsoleColor& data);
extern std::ostream& operator<<    (std::ostream& os, const ConsoleBackgroundColor& data);
extern std::wostream& operator<<   (std::wostream& os, const ConsoleColor& data);
extern std::wostream& operator<<   (std::wostream& os, const ConsoleBackgroundColor& data);

// Found more infomation https://learn.microsoft.com/zh-cn/windows/console/console-virtual-terminal-sequences

#endif // !__FILE_CONVENTION_CONSOLE_INSTANCE
