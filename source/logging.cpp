/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "logging.hpp"

#include "utils/config.hpp"

#include <memory>

std::string Logging::format(const std::string& fmt_str, ...)
{
	va_list ap;
	char* fp = NULL;
	va_start(ap, fmt_str);
	vasprintf(&fp, fmt_str.c_str(), ap);
	va_end(ap);
	std::unique_ptr<char, decltype(free)*> formatted(fp, free);
	return std::string(formatted.get());
}

std::string Logging::logDate(void)
{
	time_t unixTime;
	struct tm timeStruct;
	time(&unixTime);
	localtime_r(&unixTime, &timeStruct);
	return format("%04i-%02i-%02i %02i:%02i:%02i", timeStruct.tm_year + 1900, timeStruct.tm_mon + 1, timeStruct.tm_mday, timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec);
}

void Logging::createLogFile(void) {
	if((access("sdmc:/3ds/Universal-Updater/Log.log", F_OK) != 0)) {
		FILE* logFile = fopen(("sdmc:/3ds/Universal-Updater/Log.log"), "w");
		fclose(logFile);
	}
}

// Only write to the Log, if it is enabled in the Settings File!
void Logging::writeToLog(std::string debugText) {
	if (Config::getBool("LOGGING") == true) {
		std::ofstream logFile;
		logFile.open(("sdmc:/3ds/Universal-Updater/Log.log"), std::ofstream::app);
		std::string writeDebug = "[ ";
		writeDebug += logDate();
		writeDebug += " ] ";
		writeDebug += debugText.c_str();
		logFile << writeDebug << std::endl;
		logFile.close();
	}
}