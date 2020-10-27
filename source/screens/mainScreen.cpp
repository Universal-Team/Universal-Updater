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

#include "download.hpp"
#include "mainScreen.hpp"
#include "storeUtils.hpp"
#include <unistd.h>

MainScreen::MainScreen() {
	this->meta = std::make_unique<Meta>();

	/* Check if lastStore is accessible. */
	if (config->lastStore() != "universal-db-beta.unistore" || config->lastStore() != "") {
		if (access((std::string("sdmc:/3ds/Universal-Updater/stores/") + config->lastStore()).c_str(), F_OK) != 0) {
			config->lastStore("universal-db-beta.unistore");
		}
	}

	/* If Universal DB --> Get! */
	if (config->lastStore() == "universal-db-beta.unistore" || config->lastStore() == "") {
		if (access("sdmc:/3ds/Universal-Updater/stores/universal-db-beta.unistore", F_OK) != 0) {
			std::string tmp = "";
			DownloadUniStore("https://db.universal-team.net/unistore/universal-db-beta.unistore", -1, tmp, true, true);
			DownloadSpriteSheet("https://db.universal-team.net/unistore/universal-db.t3x", "universal-db.t3x");
		}
	}

	this->store = std::make_unique<Store>("sdmc:/3ds/Universal-Updater/stores/" + config->lastStore());
	StoreUtils::ResetAll(this->store, this->meta, this->entries);
};

void MainScreen::Draw(void) const {
	GFX::DrawTop();
	if (this->store && this->store->GetValid()) Gui::DrawStringCentered(0, 1, 0.7, TEXT_COLOR, this->store->GetUniStoreTitle());
	else Gui::DrawStringCentered(0, 1, 0.7f, TEXT_COLOR, Lang::get("INVALID_UNISTORE"), 370);
	config->list() ? StoreUtils::DrawList(this->store, this->entries) : StoreUtils::DrawGrid(this->store, this->entries);

	GFX::DrawBottom();

	switch(this->storeMode) {
		case 0:
			/* Entry Info. */
			if (this->store && this->store->GetValid() && this->entries.size() > 0) StoreUtils::DrawEntryInfo(this->store, this->entries[this->store->GetEntry()]);
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
			StoreUtils::DrawSorting(this->ascending, this->sorttype);
			break;

		case 4:
			StoreUtils::DrawSettings(this->sPage, this->sSelection);
			break;
	}

	StoreUtils::DrawSideMenu(this->storeMode);
	if (this->showMarks && this->store && this->store->GetValid()) StoreUtils::DisplayMarkBox(this->entries[this->store->GetEntry()]->GetMarks());
}

void MainScreen::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (this->showMarks) StoreUtils::MarkHandle(hDown, hHeld, touch, this->entries[this->store->GetEntry()], this->store, this->showMarks, this->meta);

	if (!this->showMarks) {
		if (this->storeMode == 0 || this->storeMode == 2 || this->storeMode == 3) {
			config->list() ? StoreUtils::ListLogic(hDown, hHeld, touch, this->store, this->entries) : StoreUtils::GridLogic(hDown, hHeld, touch, this->store, this->entries);
		}

		StoreUtils::SideMenuHandle(hDown, hHeld, touch, this->storeMode, this->fetchDown);

		/* Fetch Download list. */
		if (this->fetchDown) {
			this->fetchDown = false;
			this->dwnldList.clear();

			if (this->store && this->store->GetValid()) {
				this->store->SetDownloadIndex(0); // Reset to 0.
				this->store->SetDownloadSIndex(0);
				this->store->SetDownloadBtn(0);

				if ((int)this->entries.size() > this->store->GetEntry()) {
					this->dwnldList = this->store->GetDownloadList(this->entries[this->store->GetEntry()]->GetEntryIndex());
				}
			}
		}

		switch(this->storeMode) {
			case 0:
				if (this->store && this->store->GetValid()) StoreUtils::EntryHandle(hDown, hHeld, touch, this->showMarks, this->storeMode, this->fetchDown);
				break;

			case 1:
				if (this->store && this->store->GetValid()) StoreUtils::DownloadHandle(hDown, hHeld, touch, this->store, this->entries[this->store->GetEntry()], this->dwnldList, this->storeMode, this->meta);
				break;

			case 2:
				StoreUtils::SearchHandle(hDown, hHeld, touch, this->store, this->entries, this->searchIncludes, this->meta, this->searchResult, this->marks);
				break;

			case 3:
				StoreUtils::SortHandle(hDown, hHeld, touch, this->store, this->entries, this->ascending, this->sorttype);
				break;

			case 4:
				StoreUtils::SettingsHandle(hDown, hHeld, touch, this->sPage, this->showSettings, this->storeMode, this->sSelection, this->store, this->entries, this->meta);
				break;
		}
	}
}