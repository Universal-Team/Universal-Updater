/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 Universal-Team
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

#ifndef _UNIVERSAL_UPDATER_SETTINGS_HPP
#define _UNIVERSAL_UPDATER_SETTINGS_HPP

#include "common.hpp"
#include "fileBrowse.hpp"
#include "structs.hpp"

#include <vector>

class Settings : public Screen {
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
	Settings();
private:
	int colorMode = 0;
	int mode = 0;
	int Selection = 0;
	bool dropDownMenu = false;
	mutable int screenPos = 0;
	int colorSelection = 0;
	int keyRepeatDelay = 0;
	int settingPage = 0;

	// Draws.
	void DrawSubMenu(void) const;
	void DrawLanguageSelection(void) const;
	void DrawColorChanging(void) const;
	// Logic's.
	void SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void LanguageSelection(u32 hDown, u32 hHeld, touchPosition touch);
	void colorChanging(u32 hDown, u32 hHeld, touchPosition touch);

	const std::vector<Structs::ButtonPos> mainButtons = {
		{80, 30, 149, 52}, // Language.
		{80, 90, 149, 52}, // Colors.
		{80, 150, 149, 52} // Credits.
	};

	const std::vector<Structs::ButtonPos> mainButtons2 = {
		{0, 60, 149, 52}, // Change Music File.
		{162, 60, 149, 52}, // Change bar style.
		{0, 130, 149, 52}, // Change Key Delay.
		{162, 130, 149, 52} // Toggle animation.
	};

	const std::vector<Structs::ButtonPos> arrowPos = {
		{0, 0, 25, 25}, // Previous Arrow.
		{295, 0, 25, 25}, // Next Arrow.
		{0, 215, 25, 25}, // Back Arrow.
		{0, 0, 25, 25}, // Back Arrow top left.
		{295, 215, 25, 25}, // Next Page / Misc Arrow.
		{5, 0, 25, 25} // Dropdown Menu.
	};
};

#endif