/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "screens/screen.hpp"
#include "screens/screenCommon.hpp"

#include "utils/fileBrowse.h"
#include "utils/structs.hpp"

class UniStore : public screen
{
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
private:
	void DrawSubMenu(void) const;
	void DrawStoreList(void) const;
	void DrawStore(void) const;
	void DrawSearch(void) const;
	void DrawGitHubScreen(void) const;

	void SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void StoreSelectionLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void StoreLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void SearchLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void GitHubLogic(u32 hDown, u32 hHeld, touchPosition touch);


	void execute();
	void descript();
	int mode = 0;
	std::string selectedOptionAppStore;

	// Stuff for the GitHub Store Search function.
	std::string OwnerAndRepo = "";
	std::string fileName = "";

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
		{295, 0, 25, 25, -1}, // Arrow Up.
		{295, 215, 25, 25, -1}, // Arrow Down.
		{0, 215, 25, 25, -1}, // Back Arrow.
		{5, 0, 25, 25, -1}, // ViewMode Change.
		{45, 0, 25, 25, -1}, // Search.
		{85, 0, 25, 25, -1}, // Update.
	};

	std::vector<Structs::ButtonPos> URLBtn = {
		{10, 100, 140, 35, -1}, // FULL URL.
		{170, 100, 140, 35, -1}, // Github.
	};

	std::vector<Structs::ButtonPos> GitHubPos = {
		{30, 50, 260, 30, -1}, // Owner & Repo.
		{30, 130, 260, 30, -1}, // Filename.
		{135, 180, 50, 30, -1}, // OK.
	};

	std::vector<Structs::ButtonPos> subPos = {
		{90, 40, 140, 35, -1}, // StoreList.
		{90, 100, 140, 35, -1}, // storeSearch.
		{90, 160, 140, 35, -1}, // storePathChange.
	};
};

#endif