/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2021 Universal-Team
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

#include "animation.hpp"
#include "download.hpp"
#include "fileBrowse.hpp"
#include "mainScreen.hpp"
#include "queueSystem.hpp"
#include "screenshot.hpp"
#include "storeUtils.hpp"
#include <unistd.h>

extern int fadeAlpha;

extern UniStoreInfo GetInfo(const std::string &file, const std::string &fileName);
extern void notConnectedMsg();

/*
	MainScreen Constructor.

	Initialized meta, store and StoreEntry class and:

	- Downloads Universal-DB.. in case nothing exist.
*/
MainScreen::MainScreen() {
	StoreUtils::meta = std::make_unique<Meta>();

	/* Check if lastStore is accessible. */
	if (config->lastStore() != "universal-db.unistore" && config->lastStore() != "") {
		if (access((_STORE_PATH + config->lastStore()).c_str(), F_OK) != 0) {
			config->lastStore("universal-db.unistore");

		} else {
			/* check version and file here. */
			const UniStoreInfo info = GetInfo((_STORE_PATH + config->lastStore()), config->lastStore());

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

	StoreUtils::store = std::make_unique<Store>(_STORE_PATH + config->lastStore(), config->lastStore());
	StoreUtils::ResetAll();
	StoreUtils::SortEntries();

	// Display Release changelog for Universal-Updater.
	if (config->changelog()) {
		if (GetChangelog() == "") return;
		StoreUtils::ProcessReleaseNotes(GetChangelog());
		storeMode = 7;
	}
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

	Gui::ScreenDraw(Top);
	config->list() ? StoreUtils::DrawList() : StoreUtils::DrawGrid();
	Gui::Draw_Rect(0, 0, 400, 25, UIThemes->BarColor());
	Gui::Draw_Rect(0, 25, 400, 1, UIThemes->BarOutline());

	if (StoreUtils::store && StoreUtils::store->GetValid()) Gui::DrawStringCentered(0, 1, 0.7f, UIThemes->TextColor(), StoreUtils::store->GetUniStoreTitle(), 360, 0, font);
	else Gui::DrawStringCentered(0, 1, 0.7f, UIThemes->TextColor(), Lang::get("INVALID_UNISTORE"), 370, 0, font);
	GFX::DrawTime();
	GFX::DrawBattery();
	GFX::DrawWifi();
	Animation::QueueEntryDone();

	/* Download-ception. */
	if (this->storeMode == 1) {
		StoreUtils::DrawDownList(this->dwnldList, this->fetchDown, StoreUtils::entries[StoreUtils::store->GetEntry()], this->dwnldSizes, this->installs);

	} else {
		if (fadeAlpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, fadeAlpha));
		GFX::DrawBottom();

		switch(this->storeMode) {
			case 0:
				/* Entry Info. */
				if (StoreUtils::store && StoreUtils::store->GetValid() && StoreUtils::entries.size() > 0) StoreUtils::DrawEntryInfo(StoreUtils::entries[StoreUtils::store->GetEntry()]);
				break;

			case 2:
				/* Queue Menu. */
				StoreUtils::DrawQueueMenu(this->queueIndex);
				break;

			case 3:
				/* Search + Favorites. */
				StoreUtils::DrawSearchMenu(this->searchIncludes, this->searchResult, this->marks, this->updateFilter, this->isAND);
				break;

			case 4:
				/* Sorting. */
				StoreUtils::DrawSorting();
				break;

			case 5:
				/* Settings. */
				StoreUtils::DrawSettings(this->sPage, this->sSelection, this->sPos);
				break;
		}
	}

	StoreUtils::DrawSideMenu(this->storeMode);
	if (this->storeMode == 7) {
		/* Release Notes. */
		StoreUtils::DrawReleaseNotes(this->scrollOffset, StoreUtils::entries[StoreUtils::store->GetEntry()]);
		return;
	}
	if (this->showMarks && StoreUtils::store && StoreUtils::store->GetValid()) StoreUtils::DisplayMarkBox(StoreUtils::entries[StoreUtils::store->GetEntry()]->GetMarks());
	if (fadeAlpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(0, 0, 0, fadeAlpha));
}

/*
	MainScreen Logic.
*/
void MainScreen::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	Animation::HandleQueueEntryDone();
	GFX::HandleBattery();

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

			if (this->screenshotIndex < (int)StoreUtils::entries[StoreUtils::store->GetEntry()]->GetScreenshotNames().size()) {
				this->screenshotName = StoreUtils::entries[StoreUtils::store->GetEntry()]->GetScreenshotNames()[this->screenshotIndex];
			}

			this->sSize = 0;
			this->sSize = StoreUtils::entries[StoreUtils::store->GetEntry()]->GetScreenshots().size();

			if (this->screenshotIndex < this->sSize) {
				if (this->sSize > 0) {
					this->Screenshot = FetchScreenshot(StoreUtils::entries[StoreUtils::store->GetEntry()]->GetScreenshots()[this->screenshotIndex]);
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
		StoreUtils::ReleaseNotesLogic(this->scrollOffset, this->scrollDelta, this->storeMode);
		return;
	}

	/* Mark Menu. */
	if (this->showMarks) StoreUtils::MarkHandle(StoreUtils::entries[StoreUtils::store->GetEntry()], this->showMarks);

	if (!this->showMarks) {
		if (storeMode == 0 || storeMode == 3 || storeMode == 4) {
			config->list() ? StoreUtils::ListLogic(storeMode, this->lastMode, this->fetchDown, this->smallDelay) : StoreUtils::GridLogic(storeMode, this->lastMode, this->fetchDown, this->smallDelay);
		}

		StoreUtils::SideMenuHandle(storeMode, this->fetchDown, this->lastMode);

		/* Fetch Download list. */
		if (this->fetchDown) {
			this->installs.clear();
			this->dwnldList.clear();
			this->dwnldSizes.clear();
			this->dwnldTypes.clear();

			if (StoreUtils::store && StoreUtils::store->GetValid()) {
				const std::vector<std::string> installedNames = StoreUtils::meta->GetInstalled(StoreUtils::store->GetUniStoreTitle(), StoreUtils::entries[StoreUtils::store->GetEntry()]->GetTitle());
				StoreUtils::store->SetDownloadIndex(0); // Reset to 0.
				StoreUtils::store->SetDownloadSIndex(0);

				if ((int)StoreUtils::entries.size() > StoreUtils::store->GetEntry()) {
					this->dwnldList = StoreUtils::store->GetDownloadList(StoreUtils::entries[StoreUtils::store->GetEntry()]->GetEntryIndex());
					this->dwnldSizes = StoreUtils::entries[StoreUtils::store->GetEntry()]->GetSizes();
					this->dwnldTypes = StoreUtils::entries[StoreUtils::store->GetEntry()]->GetTypes();

					for (int i = 0; i < (int)this->dwnldList.size(); i++) {
						bool good = false;

						for (int i2 = 0; i2 < (int)installedNames.size(); i2++) {
							if (installedNames[i2] == this->dwnldList[i]) {
								this->installs.push_back( true );
								good = true;
							}
						}

						if (!good) this->installs.push_back( false );
					}
				}
			}

			this->fetchDown = false;
		}

		switch(storeMode) {
			case 0:
				if (StoreUtils::store && StoreUtils::store->GetValid() && StoreUtils::entries.size() > 0) StoreUtils::EntryHandle(this->showMarks, this->fetchDown, this->screenshotFetch, storeMode, StoreUtils::entries[StoreUtils::store->GetEntry()]);
				break;

			case 1:
				if (StoreUtils::store && StoreUtils::store->GetValid() && StoreUtils::entries.size() > 0) StoreUtils::DownloadHandle(StoreUtils::entries[StoreUtils::store->GetEntry()], this->dwnldList, storeMode, this->lastMode, this->smallDelay, this->installs, this->dwnldTypes);
				break;

			case 2:
				StoreUtils::QueueMenuHandle(this->queueIndex, this->storeMode);
				break;

			case 3:
				StoreUtils::SearchHandle(this->searchIncludes, this->searchResult, this->marks, this->updateFilter, this->isAND);
				break;

			case 4:
				StoreUtils::SortHandle();
				break;

			case 5:
				StoreUtils::SettingsHandle(this->sPage, this->showSettings, storeMode, this->sSelection, this->sPos);
				break;
		}
	}
}