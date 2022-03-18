/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2021 Universal-Team
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

#ifndef _UNIVERSAL_UPDATER_SCRIPT_UTILS_HPP
#define _UNIVERSAL_UPDATER_SCRIPT_UTILS_HPP

#include "json.hpp"
#include <3ds.h>
#include <string>

enum ScriptState {
	NONE = 0,
	FAILED_DOWNLOAD,
	SCRIPT_CANCELED,
	SYNTAX_ERROR,
	COPY_ERROR,
	MOVE_ERROR,
	DELETE_ERROR,
	EXTRACT_ERROR
};

namespace ScriptUtils {
	bool matchPattern(const std::string &pattern, const std::string &tested);

	Result removeFile(const std::string &file, bool isARG = false);
	void bootTitle(const std::string &TitleID, bool isNAND, bool isARG = false);
	Result prompt(const std::string &message);
	Result copyFile(const std::string &source, const std::string &destination, const std::string &message, bool isARG = false);
	Result renameFile(const std::string &oldName, const std::string &newName, bool isARG = false);
	Result downloadRelease(const std::string &repo, const std::string &file, const std::string &output, bool includePrereleases, const std::string &message, bool isARG = false);
	Result downloadFile(const std::string &file, const std::string &output, const std::string &message, bool isARG = false);
	void installFile(const std::string &file, bool updatingSelf, const std::string &message, bool isARG = false);
	Result extractFile(const std::string &file, const std::string &input, const std::string &output, const std::string &message, bool isARG = false);

	Result runFunctions(nlohmann::json storeJson, int selection, const std::string &entry);
};

#endif