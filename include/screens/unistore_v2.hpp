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

#ifndef UNIVERSAL_STORE_V2_HPP
#define UNIVERSAL_STORE_V2_HPP

#include "common.hpp"
#include "json.hpp"
#include "structs.hpp"
#include <citro2d.h>

#include <vector>

class UniStoreV2 : public Screen {
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
	UniStoreV2(nlohmann::json &JSON, const std::string sheetPath);
	~UniStoreV2();
private:
	bool darkMode = true, sheetLoaded = false, canDisplay = false, hasLoaded = false, isDropDown = false;
	int selectedBox = 0, lastViewMode = 0, dropSelection = 0, selectedBoxList = 0, selection = -1, storePage = 0, downloadPage = 0, storePageList = 0, mode = 0, subSelection = 0;
	nlohmann::json storeJson;
	C2D_SpriteSheet sheet;
	std::vector<std::string> objects;

	// Base stuff.
	void DrawBaseTop(void) const;
	void DrawBaseBottom(void) const;
	void drawBox(float xPos, float yPos, float width, float height, bool selected) const;
	void DrawGrid(void) const;
	void DrawDescription(int selection) const;
	u32 returnTextColor() const;
	void parseObjects(int selection);
	Result runFunctions(std::string entry);
	void DrawList(void) const;
	void displaySelectedEntry(int selection) const;
	void DropLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void DropDownMenu(void) const;

	const std::vector<Structs::ButtonPos> StoreBoxesGrid = {
		{20, 30, 100, 50},
		{150, 30, 100, 50},
		{280, 30, 100, 50},

		{20, 95, 100, 50},
		{150, 95, 100, 50},
		{280, 95, 100, 50},

		{20, 160, 100, 50},
		{150, 160, 100, 50},
		{280, 160, 100, 50}
	};

	const std::vector<Structs::ButtonPos> StoreBoxesList = {
		{20, 30, 360, 50},
		{20, 95, 360, 50},
		{20, 160, 360, 50}
	};

	const std::vector<Structs::ButtonPos> downloadBoxes = {
		{10, 40, 300, 25},
		{10, 75, 300, 25},
		{10, 110, 300, 25},
		{10, 145, 300, 25},
		{10, 180, 300, 25}
	};

	const std::vector<Structs::ButtonPos> iconPos = {
		{5, 0, 25, 25}  // Dropdown Menu.
	};

	const std::vector<Structs::ButtonPos> dropPos = {
		{5, 30, 140, 25}, // Theme.
		{5, 70, 140, 25}  // Style.
	};

	u32 barColorLight, barColorDark, bgColorLight, bgColorDark, textColorLight, textColorDark, boxColorLight, boxColorDark, outlineColorLight, outlineColorDark;
};

#endif