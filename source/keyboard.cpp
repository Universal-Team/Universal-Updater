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

#include "gfx.hpp"
#include "keyboard.hpp"
#include "screenCommon.hpp"

static std::vector<SwkbdDictWord> words;

/*
	Return a string of the keyboard.

	std::string Output: Reference to the string to write the resut to
	uint maxLength: The max length.
	const std::string &Hint: Const Reference to the hint text.
	const std::string &Text: Const Reference to the prefill text.
	const std::vector<std::shared_ptr<StoreEntry>> &entries: Const Reference of all entries for the words to suggest.

	Returns: True if "OK" was selected, False if "Cancel".
*/
bool Input::getTextKeyboard(std::string &Output, uint maxLength, const std::string &Hint, const std::string &Text, const std::vector<std::shared_ptr<StoreEntry>> &entries) {
	C3D_FrameEnd(0); // Needed, so the system will not freeze.

	SwkbdState state;
	swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, maxLength);
	swkbdSetHintText(&state, Hint.c_str());
	swkbdSetInitialText(&state, Text.c_str());

	if (entries.size()) {
		words.clear();
		words.resize(entries.size());

		for (uint i = 0; i < entries.size(); i++) {
			/* Checking for not nullptr. */
			if (entries[i]) swkbdSetDictWord(&words[i], StringUtils::lower_case(entries[i]->GetTitle()).c_str(), entries[i]->GetTitle().c_str());
			else swkbdSetDictWord(&words[i], "", "");
		}

		if (words.size() > 0) {
			swkbdSetDictionary(&state, words.data(), entries.size());
			swkbdSetFeatures(&state, SWKBD_PREDICTIVE_INPUT);
		}
	}

	char temp[maxLength + 1] = { 0 };
	SwkbdButton ret = swkbdInputText(&state, temp, sizeof(temp));
	temp[maxLength] = '\0';
	if (ret == SWKBD_BUTTON_CONFIRM) Output = temp;

	return ret == SWKBD_BUTTON_CONFIRM;
}
