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

#ifndef _UNIVERSAL_UPDATER_ACTION_HPP
#define _UNIVERSAL_UPDATER_ACTION_HPP

#include <string>


class Action {
public:
	enum class ActionType : uint8_t { None = 0, Extracting, Moving, Copying };

	virtual void Handler() = 0; // The main function that handles things.

	virtual std::pair<int, int> Files() const = 0; // The current file and amount of files, used by extraction.
	virtual std::pair<uint32_t, uint32_t> Progress() const = 0; // The current progress.
	virtual std::string CurrentFile() const = 0; // The current file. Overwrite with '""' if not used in the action.
	virtual uint8_t State() const = 0; // The current state.
	virtual ActionType Type() const = 0; // The Action Type.
	virtual bool IsDone() const = 0; // If the action is done or nah.
	virtual void Cancel() = 0; // Call this to cancel the action.
};

#endif