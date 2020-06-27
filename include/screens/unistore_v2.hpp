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

#ifndef _UNIVERSAL_UPDATER_UNIVERSAL_STORE_V2_HPP
#define _UNIVERSAL_UPDATER_UNIVERSAL_STORE_V2_HPP

#include "common.hpp"
#include "json.hpp"
#include "store.hpp"
#include "structs.hpp"
#include <citro2d.h>

#include <vector>

class UniStoreV2 : public Screen {
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
	UniStoreV2(nlohmann::json &JSON, const std::string sheetPath, const std::string fileName);
	~UniStoreV2();
private:
	std::unique_ptr<Store> sortedStore;

	bool darkMode = true, sheetLoaded = false, canDisplay = false, hasLoaded = false, isDropDown = false;
	int selectMenu = 0, selectedObject = 0, selectedBox = 0, lastViewMode = 0, dropSelection = 0, searchSelection = 0, iconAmount = 0, categorySelection = 0, selectedBoxList = 0, selection = -1, storePage = 0, downloadPage = 0, storePageList = 0, mode = 0, subSelection = 0, categoryPage = 0;
	nlohmann::json storeJson;
	C2D_SpriteSheet sheet;
	std::vector<std::string> objects;

	void DrawSortingMenu(void) const;
	void DrawSearchMenu(void) const;
	void DrawSelectMenu(int option) const;

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
	void displaySelectedEntry(int selection, int storeIndex) const;
	void DropLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void DropDownMenu(void) const;

	const std::vector<Structs::ButtonPos> StoreBoxesGrid = {
		{25, 35, 50, 50},
		{100, 35, 50, 50},
		{175, 35, 50, 50},
		{250, 35, 50, 50},
		{325, 35, 50, 50},

		{25, 95, 50, 50},
		{100, 95, 50, 50},
		{175, 95, 50, 50},
		{250, 95, 50, 50},
		{325, 95, 50, 50},

		{25, 155, 50, 50},
		{100, 155, 50, 50},
		{175, 155, 50, 50},
		{250, 155, 50, 50},
		{325, 155, 50, 50}
	};

	const std::vector<Structs::ButtonPos> StoreBoxesList = {
		{20, 35, 360, 50},
		{20, 95, 360, 50},
		{20, 155, 360, 50}
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
		{5, 70, 140, 25}, // Style.
		{5, 110, 140, 25},// Search.
		{5, 150, 140, 25} // Reset.
	};

	const std::vector<Structs::ButtonPos> sortingPos = {
		{40, 40, 100, 40}, // Descending.
		{180, 40, 100, 40}, // Ascending.
		// Now the actual options.
		{115, 90, 100, 30}, // Title.
		{115, 130, 100, 30}, // Author.
		{115, 170, 100, 30} // Last Updated.
	};

	const std::vector<Structs::ButtonPos> searchPos = {
		{0, 60, 149, 52}, // Title.
		{162, 60, 149, 52}, // Author.
		{0, 130, 149, 52}, // Category.
		{162, 130, 149, 52} // Console.
	};

	u32 barColorLight, barColorDark, bgColorLight, bgColorDark, textColorLight, textColorDark, boxColorLight, boxColorDark, outlineColorLight, outlineColorDark;
};

#endif