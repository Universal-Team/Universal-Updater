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

	/* Create Directories. */
	mkdir("sdmc:/3ds", 0x777);
	mkdir("sdmc:/3ds/Universal-Updater", 0x777);
	mkdir("sdmc:/3ds/Universal-Updater/stores", 0x777);
	mkdir("sdmc:/3ds/Universal-Updater/shortcuts", 0x777);
	
	/* Load classes. */
	this->GData = std::make_unique<GFXData>();
	this->CData = std::make_unique<ConfigData>();
	this->TData = std::make_unique<ThemeData>();
	this->MData = std::make_unique<Meta>();
	this->MSData = std::make_unique<MSGData>();

	bool DidDownload = false;
	this->MSData->DisplayWaitMsg("Checking UniStore...");
	
	/* Check if LastStore is accessible. */
	if (this->CData->LastStore() != "universal-db.unistore" && this->CData->LastStore() != "") {
		if (access((_STORE_PATH + this->CData->LastStore()).c_str(), F_OK) != 0) {
			this->CData->LastStore("universal-db.unistore");

		} else {
			/* check version and file here. */
			const UniStore::Info _Info = UniStore::GetInfo((_STORE_PATH + this->CData->LastStore()), this->CData->LastStore());

			if (_Info.Version != 3 && _Info.Version != UniStore::UNISTORE_VERSION) {
				this->CData->LastStore("universal-db.unistore");
			}

			if (_Info.File != "") { // Ensure to check for this.
				if ((_Info.File.find("/") != std::string::npos)) {
					this->CData->LastStore("universal-db.unistore"); // It does contain a '/' which is invalid.
				}
			}
		}
	}

	/* If Universal-DB --> Get! */
	if (this->CData->LastStore() == "universal-db.unistore" || this->CData->LastStore() == "") {
		if (access(_STORE_PATH "universal-db.unistore", F_OK) != 0) {
			if (DownloadUtils::WiFiAvailable()) {
				std::unique_ptr<Action> DL = std::make_unique<DownloadFile>("https://db.universal-team.net/unistore/universal-db.unistore", _STORE_PATH "universal-db.unistore");
				this->MSData->DisplayWaitMsg("Downloading universal-db.unistore...");
				DL->Handler();

				DL = nullptr;
				DL = std::make_unique<DownloadFile>("https://db.universal-team.net/unistore/universal-db.t3x", _STORE_PATH "universal-db.t3x");
				this->MSData->DisplayWaitMsg("Downloading universal-db.t3x...");
				DL->Handler();
				DidDownload = true;
			}

		} else {
			const UniStore::Info _Info = UniStore::GetInfo(_STORE_PATH "universal-db.unistore", "universal-db.unistore");

			if (_Info.Version != 3 && _Info.Version != UniStore::UNISTORE_VERSION) {
				if (DownloadUtils::WiFiAvailable()) {
					std::unique_ptr<Action> DL = std::make_unique<DownloadFile>("https://db.universal-team.net/unistore/universal-db.unistore", _STORE_PATH "universal-db.unistore");
					this->MSData->DisplayWaitMsg("Downloading universal-db.unistore...");
					DL->Handler();

					DL = nullptr;
					DL = std::make_unique<DownloadFile>("https://db.universal-team.net/unistore/universal-db.t3x", _STORE_PATH "universal-db.t3x");
					this->MSData->DisplayWaitMsg("Downloading universal-db.t3x...");
					DL->Handler();
					DidDownload = true;
				}
			}
		}
	}

	this->Store = std::make_unique<UniStore>(_STORE_PATH + this->CData->LastStore(), this->CData->LastStore(), DidDownload);

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

	this->GData->DrawBottom();
	this->_Tabs->DrawBottom();
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