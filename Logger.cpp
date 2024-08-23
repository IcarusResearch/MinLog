#include "Logger.h"

const Color Color::red(FOREGROUND_RED);
const Color Color::green(FOREGROUND_GREEN);
const Color Color::blue(FOREGROUND_BLUE);
const Color Color::yellow(FOREGROUND_RED | FOREGROUND_GREEN);
const Color Color::purple(FOREGROUND_RED | FOREGROUND_BLUE);
const Color Color::cyan(FOREGROUND_GREEN | FOREGROUND_BLUE);
const Color Color::white(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
const Color Color::gray(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
const Color Color::lightRed(FOREGROUND_RED | FOREGROUND_INTENSITY);
const Color Color::lightGreen(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
const Color Color::lightBlue(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
const Color Color::lightYellow(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
const Color Color::lightPurple(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
const Color Color::lightCyan(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

const Level Level::LEVEL_DEBUG(TXT("DEBUG"), Color::lightCyan);
const Level Level::LEVEL_INFO(TXT("INFO"), Color::lightGreen);
const Level Level::LEVEL_WARN(TXT("WARN"), Color::yellow);
const Level Level::LEVEL_ERROR(TXT("ERROR"), Color::lightRed);

void Log(CONST Level level, __PCTCH szSource, __PCTCH szFormat, ...) {
	va_list args;
	va_start(args, szFormat);
	TXTCHAR msg[MAX_LOG_LEN];
	vsntprintf(msg, MAX_LOG_LEN, szFormat, args);
	va_end(args);
	tstring szMsg(msg);
	time_t currentTime = time(nullptr);
	tm localTime;
	localtime_s(&localTime, &currentTime);
	tcout << Color::gray << TXT("[")
		<< Color::white << std::setw(2) << std::setfill(TXT('0')) << localTime.tm_hour << std::setfill(TXT(' '))
		<< Color::gray << TXT(":")
		<< Color::white << std::setw(2) << std::setfill(TXT('0')) << localTime.tm_min << std::setfill(TXT(' '))
		<< Color::gray << TXT(":")
		<< Color::white << std::setw(2) << std::setfill(TXT('0')) << localTime.tm_sec << std::setfill(TXT(' '))
		<< Color::gray << TXT("] ")
		<< Color::white << szSource << std::setw(15 - tcslen(szSource))
		<< Color::gray << TXT(" - ") << level << Color::gray << TXT(" - ");
	tstring::size_type pos = 0;
	if (szMsg.size() >= 1 && szMsg[0] != COLOR_FORMAT_CHAR) {
		szMsg.insert(0, 1, DEFAULT_COLOR);
		szMsg.insert(0, 1, COLOR_FORMAT_CHAR);
	}
	while ((pos = szMsg.find(COLOR_FORMAT_CHAR, pos)) != tstring::npos) {
		if (pos + 1 < szMsg.length() && isalnum(szMsg[pos + 1])) {
			tstring::size_type next_pos = szMsg.find(COLOR_FORMAT_CHAR, pos + 1);
			tstring sub;
			if (next_pos == tstring::npos) {
				sub = szMsg.substr(pos + 2);
			} else {
				sub = szMsg.substr(pos + 2, next_pos - pos - 2);
			}
			auto itr = charToColor.find(szMsg[pos + 1]);
			if (itr == charToColor.end()) {
				tcout << sub;
			} else {
				tcout << itr->second << sub;
			}
			pos = next_pos;
		} else {
			pos++;
		}
	}
}
