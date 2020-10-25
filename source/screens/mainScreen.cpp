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

#include "mainScreen.hpp"
#include "storeUtils.hpp"

MainScreen::MainScreen() {
	this->meta = std::make_unique<Meta>();
	this->init("sdmc:/3ds/Universal-Updater/stores/Universal-DB.unistore");
};

void MainScreen::init(const std::string &storeName) {
	this->storeMode = 0;
	Msg::DisplayMsg("Preparing UniStore... please wait.");

	this->store = nullptr;
	this->initialized = false;

	this->store = std::make_unique<Store>(storeName);

	StoreUtils::ResetAll(this->store, this->meta, this->entries);
	this->initialized = true;
}

void MainScreen::Draw(void) const {
	/* If not initialized --> Bruh. */
	if (!this->initialized) {
		GFX::DrawTop();
		Gui::DrawStringCentered(0, 1, 0.7, C2D_Color32(255, 255, 255, 255), "UniStore not initialized.");
		GFX::DrawBottom();
		return;
	}

	GFX::DrawTop();

	Gui::DrawStringCentered(0, 1, 0.7, C2D_Color32(255, 255, 255, 255), this->store->GetUniStoreTitle());
	StoreUtils::DrawGrid(this->store, this->entries);

	GFX::DrawBottom();

	switch(this->storeMode) {
		case 0:
			/* Entry Info. */
			StoreUtils::DrawEntryInfo(this->store, this->entries[this->store->GetEntry()]);
			break;

		case 1:
			/* Download List. */
			StoreUtils::DrawDownList(this->store, this->dwnldList);
			break;

		case 2:
			/* Search + Favorites. */
			StoreUtils::DrawSearchMenu(this->searchIncludes, this->searchResult, this->marks);
			break;

		case 3:
			/* Sorting. */
			break;

		case 4:
			/* Credits + Settings(?). */
			break;
	}

	StoreUtils::DrawSideMenu(this->storeMode);
	if (this->showMarks) StoreUtils::DisplayMarkBox(this->entries[this->store->GetEntry()]->GetMarks());
}

void MainScreen::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (!this->initialized) {
		if (hDown & KEY_Y) this->init("sdmc:/3ds/Universal-Updater/stores/Universal-DB.unistore");
		return;
	}

	if (this->showMarks) StoreUtils::MarkHandle(hDown, hHeld, touch, this->entries[this->store->GetEntry()], this->store, this->showMarks, this->meta);

	if (!this->showMarks) {
		if (this->storeMode != 1) StoreUtils::GridLogic(hDown, hHeld, touch, this->store, this->entries);

		StoreUtils::SideMenuHandle(hDown, hHeld, touch, this->storeMode, this->fetchDown);

		/* Fetch Download list. */
		if (this->fetchDown) {
			this->fetchDown = false;

			this->dwnldList.clear();
			this->dwnldList = this->store->GetDownloadList(this->entries[this->store->GetEntry()]->GetEntryIndex());
			this->store->SetDownloadIndex(0); // Reset to 0.
			this->store->SetDownloadSIndex(0);
			this->store->SetDownloadBtn(0);
		}

		switch(this->storeMode) {
			case 0:
				StoreUtils::EntryHandle(hDown, hHeld, touch, this->showMarks);
				break;

			case 1:
				StoreUtils::DownloadHandle(hDown, hHeld, touch, this->store, this->dwnldList);
				break;

			case 2:
				StoreUtils::SearchHandle(hDown, hHeld, touch, this->store, this->entries, this->searchIncludes, this->meta, this->searchResult, this->marks);
				break;
		}


		if (hDown & KEY_Y) this->init("sdmc:/3ds/Universal-Updater/stores/Universal-DB.unistore");
	}
}