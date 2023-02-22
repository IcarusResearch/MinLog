#pragma once

#include <Windows.h>
#include <ctime>
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

static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

class Color {
private:
	WORD color;
public:
	Color(int color) : color(color) {}

	friend std::ostream& operator << (std::ostream& os, const Color& c) {
		SetConsoleTextAttribute(hConsole, c.color);
		return os;
	}

	static const Color red, green, blue, yellow, purple, cyan, white, gray,
		lightRed, lightGreen, lightBlue, lightYellow, lightPurple, lightCyan;
};

class Level {
private:
	std::string name;
	Color color;
public:
	Level(std::string name, Color color) : name(name), color(color) {}

	friend std::ostream& operator << (std::ostream& os, const Level& level) {
		return os << level.color << level.name << std::setw(8 - level.name.length()) << Color::gray;
	}

	static const Level LEVEL_DEBUG, LEVEL_INFO, LEVEL_WARN, LEVEL_ERROR;
};

const inline Color Color::red(FOREGROUND_RED);
const inline Color Color::green(FOREGROUND_GREEN);
const inline Color Color::blue(FOREGROUND_BLUE);
const inline Color Color::yellow(FOREGROUND_RED | FOREGROUND_GREEN);
const inline Color Color::purple(FOREGROUND_RED | FOREGROUND_BLUE);
const inline Color Color::cyan(FOREGROUND_GREEN | FOREGROUND_BLUE);
const inline Color Color::white(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
const inline Color Color::gray(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
const inline Color Color::lightRed(FOREGROUND_RED | FOREGROUND_INTENSITY);
const inline Color Color::lightGreen(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
const inline Color Color::lightBlue(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
const inline Color Color::lightYellow(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
const inline Color Color::lightPurple(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
const inline Color Color::lightCyan(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

static const std::map<char, Color> charToColor {
	{'a', Color::red},
	{'b', Color::green},
	{'c', Color::blue},
	{'d', Color::yellow},
	{'e', Color::purple},
	{'f', Color::cyan},
	{'g', Color::white},
	{'h', Color::gray},

	{'1', Color::lightRed},
	{'2', Color::lightGreen},
	{'3', Color::lightBlue},
	{'4', Color::lightYellow},
	{'5', Color::lightPurple},
	{'6', Color::lightCyan},
};

const inline Level Level::LEVEL_DEBUG("DEBUG", Color::lightCyan);
const inline Level Level::LEVEL_INFO("INFO", Color::lightGreen);
const inline Level Level::LEVEL_WARN("WARN", Color::yellow);
const inline Level Level::LEVEL_ERROR("ERROR", Color::lightRed);

inline void Log(CONST Level level, PCCH szSource, PCCH szFormat, ...) {
	va_list args;
	va_start(args, szFormat);
	CHAR msg[MAX_LOG_LEN];
	vsnprintf(msg, MAX_LOG_LEN, szFormat, args);
	va_end(args);
	std::string szMsg(msg);
	time_t currentTime = time(nullptr);
	tm localTime;
	localtime_s(&localTime, &currentTime);
	std::cout << std::dec << Color::gray << "["
		<< Color::white << std::setw(2) << std::setfill('0') << localTime.tm_hour << std::setfill(' ')
		<< Color::gray << ":"
		<< Color::white << std::setw(2) << std::setfill('0') << localTime.tm_min << std::setfill(' ')
		<< Color::gray << ":"
		<< Color::white << std::setw(2) << std::setfill('0') << localTime.tm_sec << std::setfill(' ')
		<< Color::gray << "] "
		<< Color::white << szSource << std::setw(15 - strlen(szSource))
		<< Color::gray << " - " << level << Color::gray << " - ";
	SIZE_T pos = 0;
	if (szMsg.size() >= 1 && szMsg[0] != COLOR_FORMAT_CHAR) {
		szMsg.insert(0, 1, DEFAULT_COLOR);
		szMsg.insert(0, 1, COLOR_FORMAT_CHAR);
	}
	while ((pos = szMsg.find(COLOR_FORMAT_CHAR, pos)) != std::string::npos) {
		if (pos + 1 < szMsg.length() && isalpha(szMsg[pos + 1])) {
			SIZE_T next_pos = szMsg.find(COLOR_FORMAT_CHAR, pos + 1);
			std::string sub = szMsg.substr(pos + 2, next_pos - pos - 2);
			auto itr = charToColor.find(szMsg[pos + 1]);
			if (itr == charToColor.end()) {
				std::cout << sub;
			} else {
				std::cout << charToColor.find(szMsg[pos + 1])->second << sub;
			}
			pos = next_pos;
		} else {
			pos++;
		}
	}
}

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
