#pragma once

#include <Windows.h>
#include <ctime>
#include <tchar.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>

#define MAX_LOG_LEN 300
#define COLOR_FORMAT_CHAR '~'
#define DEFAULT_COLOR 'g'
#ifndef LOG_LEVEL_ERROR
#define LOG_LEVEL_ERROR
#endif

#if defined(UNICODE) && !defined(FORCE_MULTIBYTE)
#define tostream std::wostream
#define tstring std::wstring
#define tcout std::wcout
#define vsntprintf _vsnwprintf_s
#define tcslen wcslen
#define TXT(str) L ## str
typedef LPCWCH __PCTCH;
typedef WCHAR TXTCHAR;
#else
#define tostream std::ostream
#define tstring std::string
#define tcout std::cout
#define vsntprintf vsnprintf
#define tcslen strlen
#define TXT(str) str
typedef LPCCH __PCTCH;
typedef CHAR TXTCHAR;
#endif

static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

class Color {
private:
	WORD color;
public:
	Color(int color) : color(color) {}

	friend tostream& operator << (tostream& os, const Color& c) {
		SetConsoleTextAttribute(hConsole, c.color);
		return os;
	}

	static const Color red, green, blue, yellow, purple, cyan, white, gray,
		lightRed, lightGreen, lightBlue, lightYellow, lightPurple, lightCyan;
};

class Level {
private:
	tstring name;
	Color color;
public:
	Level(tstring name, Color color) : name(name), color(color) {}

	friend tostream& operator << (tostream& os, const Level& level) {
		return os << level.color << level.name << std::setw(8 - level.name.length()) << Color::gray;
	}

	static const Level LEVEL_DEBUG, LEVEL_INFO, LEVEL_WARN, LEVEL_ERROR;
};

static const std::map<TCHAR, Color> charToColor{
	{TXT('a'), Color::red},
	{TXT('b'), Color::green},
	{TXT('c'), Color::blue},
	{TXT('d'), Color::yellow},
	{TXT('e'), Color::purple},
	{TXT('f'), Color::cyan},
	{TXT('g'), Color::white},
	{TXT('h'), Color::gray},

	{TXT('1'), Color::lightRed},
	{TXT('2'), Color::lightGreen},
	{TXT('3'), Color::lightBlue},
	{TXT('4'), Color::lightYellow},
	{TXT('5'), Color::lightPurple},
	{TXT('6'), Color::lightCyan},
};

void Log(CONST Level level, __PCTCH szSource, __PCTCH szFormat, ...);

#ifdef LOG_LEVEL_DEBUG
#define LOG_DEBUG_ENABLED
#define LOG_INFO_ENABLED
#define LOG_WARN_ENABLED
#define LOG_ERROR_ENABLED
#elif defined(LOG_LEVEL_INFO)
#define LOG_INFO_ENABLED
#define LOG_WARN_ENABLED
#define LOG_ERROR_ENABLED
#elif defined(LOG_LEVEL_WARN)
#define LOG_WARN_ENABLED
#define LOG_ERROR_ENABLED
#elif defined(LOG_LEVEL_ERROR)
#define LOG_ERROR_ENABLED
#endif

#ifdef LOG_DEBUG_ENABLED
#define LOG_DEBUG(source, fmt, ...)		Log(Level::LEVEL_DEBUG, source, fmt, ##__VA_ARGS__);
#else
#define LOG_DEBUG(source, fmt, ...)
#endif

#ifdef LOG_INFO_ENABLED
#define LOG_INFO(source, fmt, ...)		Log(Level::LEVEL_INFO, source, fmt, ##__VA_ARGS__);
#else
#define LOG_INFO(source, fmt, ...)
#endif

#ifdef LOG_WARN_ENABLED
#define LOG_WARN(source, fmt, ...)		Log(Level::LEVEL_WARN, source, fmt, ##__VA_ARGS__);
#else
#define LOG_WARN(source, fmt, ...)
#endif

#ifdef LOG_ERROR_ENABLED
#define LOG_ERROR(source, fmt, ...)		Log(Level::LEVEL_ERROR, source, fmt, ##__VA_ARGS__);
#else
#define LOG_ERROR(source, fmt, ...)
#endif
