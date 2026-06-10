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

#ifndef _UNIVERSAL_UPDATER_KEYBOARD_HPP
#define _UNIVERSAL_UPDATER_KEYBOARD_HPP

#include "storeEntry.hpp"
#include <string>

namespace Input {
	std::string setkbdString(uint maxLength, const std::string &Text, const std::vector<std::unique_ptr<StoreEntry>> &entries);

	/**
	 * Get text input from the user using the on-screen keyboard.
	 * @param initial The initial text to display in the keyboard input field.
	 * @param out A reference to a string where the user's input will be stored.
	 * @param kbdType The type of keyboard to display.
	 * @param lengthLimit The maximum length of the input string.
	 * @return True if the user confirmed their input, false if they canceled.
	 * 
	 * kbdType: 0=Default, 1=Qwerty, 2=Numpad, 3=Western
	 */
	bool getTextInput(std::string initial, std::string &out, int kbdType, int lengthLimit);
};

#endif