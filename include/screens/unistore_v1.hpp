/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#ifndef UNIVERSAL_STORE_V1_HPP
#define UNIVERSAL_STORE_V1_HPP

#include "common.hpp"
#include "json.hpp"
#include "structs.hpp"
#include <citro2d.h>

#include <vector>

class UniStoreV1 : public Screen {
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
	UniStoreV1(nlohmann::json &JSON, const std::string sheetPath, bool displayInf);
	~UniStoreV1();
private:
	// Selections.
	mutable int Selection = 0;
	bool sheetHasLoaded = false;
	int screenPos = 0;
	mutable int screenPosList = 0;
	bool dropDownMenu = false;
	int dropSelection = 0;
	int keyRepeatDelay = 0;
	nlohmann::json storeJson;
	C2D_SpriteSheet sheet;
	std::vector<std::string> objects;
	mutable bool displayInformations = true;
	void drawBlend(int key, int x, int y) const;
	void parseObjects();
	Result execute();

	// Icon | Button Structs.
	std::vector<Structs::ButtonPos> arrowPos = {
		{295, 0, 25, 25}, // Arrow Up.
		{295, 215, 25, 25}, // Arrow Down.
		{0, 215, 25, 25}, // Back Arrow.
		{5, 0, 25, 25}  // Dropdown Menu.
	};
	// DropDownMenu.
	std::vector<Structs::ButtonPos> dropPos = {
		{5, 30, 25, 25}, // Delete.
		{5, 70, 25, 25}, // Update.
		{5, 110, 25, 25} // ViewMode.
	};
	std::vector<Structs::ButtonPos> dropPos2 = {
		{0, 28, 140, 30}, // Delete.
		{0, 68, 140, 30}, // Update.
		{0, 108, 140, 30} // ViewMode.
	};
};

#endif