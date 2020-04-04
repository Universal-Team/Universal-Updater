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

#ifndef UNISTORE_HPP
#define UNISTORE_HPP

#include "common.hpp"
#include "fileBrowse.hpp"
#include "scriptHelper.hpp"
#include "structs.hpp"

#include <vector>

class UniStore : public Screen
{
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
	UniStore();
private:
	void refreshList();
	nlohmann::json openStoreFile();
	void loadStoreDesc(void);
	void loadStoreSheet(int pos);

	void DrawSubMenu(void) const;
	void DrawStoreList(void) const;
	void DrawStore(void) const;
	void DrawSearch(void) const;
	void DrawFullURLScreen(void) const;
	void DrawGitHubScreen(void) const;
	void DrawGlossary(void) const;

	void SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void StoreSelectionLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void StoreLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void SearchLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void FullURLLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void GitHubLogic(u32 hDown, u32 hHeld, touchPosition touch);


	Result execute();
	void descript();
	void updateStore(int selectedStore);
	void deleteStore(int selectedStore);
	bool handleIfDisplayText();
	int mode = 0;
	int lastMode = 1;
	mutable bool displayInformations = true;

	// Stuff for the GitHub Store Search function and Full URL.
	std::string OwnerAndRepo = "";
	std::string fileName = "";
	std::string FullURL = "";

	// Selections.
	mutable int Selection = 0;
	int screenPos = 0;
	mutable int screenPosList = 0;
	bool dropDownMenu = false;
	int dropSelection = 0;

	// Browse stuff.
	int keyRepeatDelay = 0;
	int fastMode = false;
	std::vector<DirEntry> dirContents;

	// Other stuff.
	std::vector<StoreInfo> storeInfo; // Store Selection.
	std::vector<std::string> appStoreList; // Actual store. ;P
	std::vector<std::string> descLines;
	std::string storeDesc = "";
	nlohmann::json appStoreJson;
	std::string currentStoreFile;

	// Icon | Button Structs.
	std::vector<Structs::ButtonPos> arrowPos = {
		{295, 0, 25, 25}, // Arrow Up.
		{295, 215, 25, 25}, // Arrow Down.
		{0, 215, 25, 25}, // Back Arrow.
		{5, 0, 25, 25}, // Dropdown Menu.
	};

	std::vector<Structs::ButtonPos> URLBtn = {
		{0, 60, 149, 52}, // FULL URL.
		{162, 60, 149, 52}, // GitHub.
		{0, 130, 149, 52}, // TinyDB.
		{162, 130, 149, 52} // QR Code?
	};

	std::vector<Structs::ButtonPos> GitHubPos = {
		{30, 50, 260, 30}, // Owner & Repo.
		{30, 130, 260, 30}, // Filename.
		{135, 180, 50, 30} // OK.
	};

	std::vector<Structs::ButtonPos> subPos = {
		{80, 30, 149, 52}, // StoreList.
		{80, 90, 149, 52}, // storeSearch.
		{80, 150, 149, 52} // storePathChange.
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