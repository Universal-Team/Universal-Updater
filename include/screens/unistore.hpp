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
#include "structs.hpp"

#include <vector>

class UniStore : public Screen
{
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
private:
	void DrawSubMenu(void) const;
	void DrawStoreList(void) const;
	void DrawStore(void) const;
	void DrawSearch(void) const;
	void DrawFullURLScreen(void) const;
	void DrawGitHubScreen(void) const;

	void SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void StoreSelectionLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void StoreLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void SearchLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void FullURLLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void GitHubLogic(u32 hDown, u32 hHeld, touchPosition touch);


	void execute();
	void descript();
	void updateStore(int selectedStore);
	void deleteStore(int selectedStore);
	bool handleIfDisplayText();
	int mode = 0;
	int selectedOptionAppStore = 0;
	mutable bool displayInformations = true;

	// Stuff for the GitHub Store Search function and Full URL.
	std::string OwnerAndRepo = "";
	std::string fileName = "";
	std::string FullURL = "";

	mutable int selection = 0;
	mutable int selection2 = 0;
	int screenPos = 0;
	int screenPos2 = 0;
	mutable int screenPosList = 0;
	mutable int screenPosList2 = 0;

	int searchSelection = 0;
	int subSelection = 0;

	int keyRepeatDelay = 0;
	int fastMode = false;
	std::vector<DirEntry> dirContents;

	std::vector<Structs::ButtonPos> arrowPos = {
		{295, 0, 25, 25}, // Arrow Up.
		{295, 215, 25, 25}, // Arrow Down.
		{0, 215, 25, 25}, // Back Arrow.
		{5, 0, 25, 25}, // ViewMode Change.
		{45, 0, 25, 25}, // Search.
		{85, 0, 25, 25}, // Update.
	};

	std::vector<Structs::ButtonPos> URLBtn = {
		{10, 70, 140, 40}, // FULL URL.
		{170, 70, 140, 40}, // Github.
		{10, 145, 140, 40}, // TinyDB.
		{170, 145, 140, 40}, // QR Code?
	};

	std::vector<Structs::ButtonPos> GitHubPos = {
		{30, 50, 260, 30}, // Owner & Repo.
		{30, 130, 260, 30}, // Filename.
		{135, 180, 50, 30}, // OK.
	};

	std::vector<Structs::ButtonPos> subPos = {
		{90, 40, 140, 35}, // StoreList.
		{90, 100, 140, 35}, // storeSearch.
		{90, 160, 140, 35}, // storePathChange.
	};
};

#endif