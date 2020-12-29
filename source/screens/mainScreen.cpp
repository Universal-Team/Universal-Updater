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
#include "fileBrowse.hpp"
#include "mainScreen.hpp"
#include "screenshot.hpp"
#include "storeUtils.hpp"
#include <unistd.h>

extern int fadeAlpha;

extern UniStoreInfo GetInfo(const std::string &file, const std::string &fileName);
extern void notConnectedMsg();
extern void DisplayChangelog();

/*
	MainScreen Constructor.

	Initialized Meta, Store and StoreEntry class and:

	- Downloads Universal-DB.. in case nothing exist.
*/
MainScreen::MainScreen() {
	this->meta = std::make_unique<Meta>();

	/* Check if lastStore is accessible. */
	if (config->lastStore() != "universal-db.unistore" || config->lastStore() != "") {
		if (access((std::string(_STORE_PATH) + config->lastStore()).c_str(), F_OK) != 0) {
			config->lastStore("universal-db.unistore");

		} else {
			/* check version and file here. */
			const UniStoreInfo info = GetInfo((std::string(_STORE_PATH) + config->lastStore()), config->lastStore());

			if (info.Version != 3 && info.Version != _UNISTORE_VERSION) {
				config->lastStore("universal-db.unistore");
			}

			if (info.File != "") { // Ensure to check for this.
				if ((info.File.find("/") != std::string::npos)) {
					config->lastStore("universal-db.unistore"); // It does contain a '/' which is invalid.
				}
			}
		}
	}

	/* If Universal DB --> Get! */
	if (config->lastStore() == "universal-db.unistore" || config->lastStore() == "") {
		if (access("sdmc:/3ds/Universal-Updater/stores/universal-db.unistore", F_OK) != 0) {
			if (checkWifiStatus()) {
				std::string tmp = ""; // Just a temp.
				DownloadUniStore("https://db.universal-team.net/unistore/universal-db.unistore", -1, tmp, true, true);
				DownloadSpriteSheet("https://db.universal-team.net/unistore/universal-db.t3x", "universal-db.t3x");

			} else {
				notConnectedMsg();
			}

		} else {
			const UniStoreInfo info = GetInfo("sdmc:/3ds/Universal-Updater/stores/universal-db.unistore", "universal-db.unistore");

			if (info.Version != 3 && info.Version != _UNISTORE_VERSION) {
				Msg::waitMsg("Not passing the check!");
				if (checkWifiStatus()) {
					std::string tmp = ""; // Just a temp.
					DownloadUniStore("https://db.universal-team.net/unistore/universal-db.unistore", -1, tmp, true, true);
					DownloadSpriteSheet("https://db.universal-team.net/unistore/universal-db.t3x", "universal-db.t3x");

				} else {
					notConnectedMsg();
				}
			}
		}
	}

	this->store = std::make_unique<Store>(_STORE_PATH + config->lastStore(), config->lastStore());
	StoreUtils::ResetAll(this->store, this->meta, this->entries);
	StoreUtils::SortEntries(false, SortType::LAST_UPDATED, this->entries);
	DisplayChangelog();
};

/*
	MainScreen Main Draw.
*/
void MainScreen::Draw(void) const {
	if (this->storeMode == 6) {
		/* Screenshot Menu. */
		StoreUtils::DrawScreenshotMenu(this->Screenshot, this->screenshotIndex, this->screenshotFetch, this->sSize, this->screenshotName, this->zoom, this->canDisplay);
		return;
	}

	if (this->storeMode == 7) {
		/* Release Notes. */
		StoreUtils::DrawReleaseNotes(this->scrollIndex, this->entries[this->store->GetEntry()], this->store);
		GFX::DrawBottom();
		return;
	}

	Gui::ScreenDraw(Top);
	Gui::Draw_Rect(0, 0, 400, 25, BAR_COLOR);
	Gui::Draw_Rect(0, 25, 400, 1, BAR_OUTL_COLOR);

	if (this->store && this->store->GetValid()) Gui::DrawStringCentered(0, 1, 0.7f, TEXT_COLOR, this->store->GetUniStoreTitle(), 370, 0, font);
	else Gui::DrawStringCentered(0, 1, 0.7f, TEXT_COLOR, Lang::get("INVALID_UNISTORE"), 370, 0, font);
	config->list() ? StoreUtils::DrawList(this->store, this->entries) : StoreUtils::DrawGrid(this->store, this->entries);

	/* Download-ception. */
	if (this->storeMode == 1) {
		StoreUtils::DrawDownList(this->store, this->dwnldList, this->fetchDown, this->entries[this->store->GetEntry()], this->dwnldSizes);

	} else {
		if (fadeAlpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, fadeAlpha));
		GFX::DrawBottom();

		switch(this->storeMode) {
			case 0:
				/* Entry Info. */
				if (this->store && this->store->GetValid() && this->entries.size() > 0) StoreUtils::DrawEntryInfo(this->store, this->entries[this->store->GetEntry()]);
				break;

			case 2:
				StoreUtils::DrawQueueMenu(this->queueIndex);
				break;

			case 3:
				/* Search + Favorites. */
				StoreUtils::DrawSearchMenu(this->searchIncludes, this->searchResult, this->marks, this->updateFilter);
				break;

			case 4:
				/* Sorting. */
				StoreUtils::DrawSorting(this->ascending, this->sorttype);
				break;

			case 5:
				/* Settings. */
				StoreUtils::DrawSettings(this->sPage, this->sSelection, this->sPos);
				break;
		}
	}

	StoreUtils::DrawSideMenu(this->storeMode);
	if (this->showMarks && this->store && this->store->GetValid()) StoreUtils::DisplayMarkBox(this->entries[this->store->GetEntry()]->GetMarks());
	if (fadeAlpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(0, 0, 0, fadeAlpha));
}

/*
	MainScreen Logic.
*/
void MainScreen::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	/* Screenshots Menu. */
	if (this->storeMode == 6) {
		if (this->screenshotFetch) {
			/* Delete Texture first. */
			if (this->Screenshot.tex) {
				C3D_TexDelete(this->Screenshot.tex);
				this->Screenshot.tex = nullptr;
				this->Screenshot.subtex = nullptr;
			}

			this->screenshotName = "";

			if (this->screenshotIndex < (int)this->entries[this->store->GetEntry()]->GetScreenshotNames().size()) {
				this->screenshotName = this->entries[this->store->GetEntry()]->GetScreenshotNames()[this->screenshotIndex];
			}

			this->sSize = 0;
			this->sSize = this->entries[this->store->GetEntry()]->GetScreenshots().size();

			if (this->screenshotIndex < this->sSize) {
				if (this->sSize > 0) {
					this->Screenshot = FetchScreenshot(this->entries[this->store->GetEntry()]->GetScreenshots()[this->screenshotIndex]);
					if (this->Screenshot.tex) this->canDisplay = true;
					else this->canDisplay = false;
				}
			}

			this->screenshotFetch = false;
		}

		StoreUtils::ScreenshotMenu(this->Screenshot, this->screenshotIndex, this->screenshotFetch, this->storeMode, this->sSize, this->zoom, this->canDisplay);
		return;
	}

	/* Release Notes. */
	if (this->storeMode == 7) {
		StoreUtils::ReleaseNotesLogic(this->scrollIndex, this->storeMode);
		return;
	}

	/* Mark Menu. */
	if (this->showMarks) StoreUtils::MarkHandle(this->entries[this->store->GetEntry()], this->store, this->showMarks, this->meta);

	if (!this->showMarks) {
		if (this->storeMode == 0 || this->storeMode == 3 || this->storeMode == 4) {
			config->list() ? StoreUtils::ListLogic(this->store, this->entries, this->storeMode, this->lastMode, this->fetchDown, this->smallDelay) : StoreUtils::GridLogic(this->store, this->entries, this->storeMode, this->lastMode, this->fetchDown, this->smallDelay);
		}

		StoreUtils::SideMenuHandle(this->storeMode, this->fetchDown, this->lastMode);

		/* Fetch Download list. */
		if (this->fetchDown) {
			this->dwnldList.clear();
			this->dwnldSizes.clear();

			if (this->store && this->store->GetValid()) {
				this->store->SetDownloadIndex(0); // Reset to 0.
				this->store->SetDownloadSIndex(0);

				if ((int)this->entries.size() > this->store->GetEntry()) {
					this->dwnldList = this->store->GetDownloadList(this->entries[this->store->GetEntry()]->GetEntryIndex());
					this->dwnldSizes = this->entries[this->store->GetEntry()]->GetSizes();
				}
			}

			this->fetchDown = false;
		}

		switch(this->storeMode) {
			case 0:
				if (this->store && this->store->GetValid() && this->entries.size() > 0) StoreUtils::EntryHandle(this->showMarks, this->fetchDown, this->screenshotFetch, this->storeMode, this->entries[this->store->GetEntry()]);
				break;

			case 1:
				if (this->store && this->store->GetValid() && this->entries.size() > 0) StoreUtils::DownloadHandle(this->store, this->entries[this->store->GetEntry()], this->dwnldList, this->storeMode, this->meta, this->lastMode, this->smallDelay);
				break;

			case 2:
				StoreUtils::QueueMenuHandle(this->queueIndex);
				break;

			case 3:
				StoreUtils::SearchHandle(this->store, this->entries, this->searchIncludes, this->meta, this->searchResult, this->marks, this->updateFilter, this->ascending, this->sorttype);
				break;

			case 4:
				StoreUtils::SortHandle(this->store, this->entries, this->ascending, this->sorttype);
				break;

			case 5:
				StoreUtils::SettingsHandle(this->sPage, this->showSettings, this->storeMode, this->sSelection, this->store, this->entries, this->meta, this->sPos);
				break;
		}
	}
}