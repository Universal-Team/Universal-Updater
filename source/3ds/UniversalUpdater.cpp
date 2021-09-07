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


#include "DownloadFile.hpp"
#include "DownloadUtils.hpp"
#include "UniversalUpdater.hpp"
#include <dirent.h>
#include <unistd.h>


/*
	Initialize everything as needed.
*/
void UU::Initialize() {
	romfsInit();
	gfxInitDefault();
	Gui::init();
	acInit();
	hidSetRepeatParameters(20, 8);
	osSetSpeedupEnable(true); // Enable speed-up for New 3DS users.

	/* Create Directories. */
	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/Universal-Updater", 0777);
	mkdir("sdmc:/3ds/Universal-Updater/stores", 0777);
	mkdir("sdmc:/3ds/Universal-Updater/shortcuts", 0777);
	
	/* Load classes. */
	this->GData = std::make_unique<GFXData>();
	this->CData = std::make_unique<ConfigData>();
	this->TData = std::make_unique<ThemeData>();
	this->MData = std::make_unique<Meta>();
	this->MSData = std::make_unique<MSGData>();
	this->USelector = std::make_unique<UniStoreSelector>();

	this->MSData->DisplayWaitMsg("Checking UniStore...");
	
	/* Check if LastStore is accessible. */
	if (this->CData->LastStore() == "" || access((_STORE_PATH + this->CData->LastStore()).c_str(), F_OK) != 0) {
		if (access(_STORE_PATH "universal-db.unistore", F_OK) != 0) {
			if (DownloadUtils::WiFiAvailable()) {
				std::unique_ptr<Action> DL = std::make_unique<DownloadFile>("https://db.universal-team.net/unistore/universal-db.unistore", _STORE_PATH "universal-db.unistore");
				this->MSData->DisplayWaitMsg("Downloading universal-db.unistore...");
				DL->Handler();

				DL = nullptr;
				DL = std::make_unique<DownloadFile>("https://db.universal-team.net/unistore/universal-db.t3x", _STORE_PATH "universal-db.t3x");
				this->MSData->DisplayWaitMsg("Downloading universal-db.t3x...");
				DL->Handler();
			}

		} else {
			this->CData->LastStore("universal-db.unistore");
		}
	}

	this->Store = std::make_unique<UniStore>(this->CData->LastStore());

	this->_Tabs = std::make_unique<Tabs>();
	this->TGrid = std::make_unique<TopGrid>();
	this->TList = std::make_unique<TopList>();
};


/*
	Scan the key input.
*/
void UU::ScanInput() {
	hidScanInput();
	this->Down = hidKeysDown();
	this->Repeat = hidKeysDownRepeat();
	hidTouchRead(&this->T);
};


/*
	Draws Universal-Updater's UI.
*/
void UU::Draw() {
	this->GData->StartFrame();
	this->GData->DrawTop();

	if (!this->USelector->Done) this->USelector->DrawTop();
	else {
		/* Ensure it isn't a nullptr. */
		if (this->Store && this->Store->UniStoreValid()) {
			Gui::DrawStringCentered(0, 3, TEXT_LARGE, TEXT_COLOR, this->Store->GetUniStoreTitle(), 390);

			switch(this->TMode) {
				case TopMode::Grid:
					this->TGrid->Draw();
					break;

				case TopMode::List:
					this->TList->Draw();
					break;
			}

			this->_Tabs->DrawTop();

		} else {
			Gui::DrawStringCentered(0, 3, TEXT_LARGE, TEXT_COLOR, "Invalid UniStore", 390);
		}
	}

	this->GData->DrawBottom();
	if (!this->USelector->Done) this->USelector->DrawBottom();
	else this->_Tabs->DrawBottom();
	this->GData->EndFrame();
};


/*
	Main Handler of the app. Handle Input and display stuff here.
*/
int UU::Handler() {
	this->Initialize();

	while(aptMainLoop() && !this->Exiting) {
		this->Draw();
		this->ScanInput();

		if (!this->USelector->Done) this->USelector->Handler();
		else {
			this->_Tabs->Handler(); // Tabs are always handled.

			/* Handle Top List if possible. */
			if (this->_Tabs->HandleTopScroll()) {
				switch(this->TMode) {
					case TopMode::Grid:
						this->TGrid->Handler();
						break;

					case TopMode::List:
						this->TList->Handler();
						break;
				}
			}
		}
	}

	this->CData->Sav();
	acExit();
	Gui::exit();
	gfxExit();
	romfsExit();

	return 0;
};


void UU::SwitchTopMode(const UU::TopMode TMode) {
	this->TMode = TMode;

	switch(this->TMode) {
		case TopMode::Grid:
			this->TGrid->Update();
			break;

		case TopMode::List:
			this->TList->Update();
			break;
	}
};