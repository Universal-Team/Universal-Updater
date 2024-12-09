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

#include "UniversalUpdater.hpp"

#include "DownloadFile.hpp"
#include "DownloadUtils.hpp"
#include "gui.hpp"
#include "Platform.hpp"
#include "Utils.hpp"

/*
	Initialize everything as needed.
*/
void UU::Initialize(char *ARGV[]) {
	Platform::Initialize(ARGV);

	/* Create Directories. */
	Utils::MakeDirs(_STORE_PATH);
	Utils::MakeDirs(_SHORTCUT_PATH);

	/* Load classes. */
	this->GData = std::make_unique<GFXData>();
	this->CData = std::make_unique<ConfigData>();
	this->TData = std::make_unique<ThemeData>();
	this->MData = std::make_unique<Meta>();
	this->MSData = std::make_unique<MSGData>();
	this->USelector = std::make_unique<UniStoreSelector>();

	/* Initialize Wi-Fi. */
	this->GData->StartFrame();
	this->GData->DrawTop();
	Gui::DrawStringCentered(0, 3, TEXT_LARGE, TEXT_COLOR, "Connecting to Wi-Fi...");
	this->GData->DrawBottom();
	this->GData->EndFrame();

	Platform::WiFi::Init();
	while(!Platform::WiFi::Connected()) {
		Platform::waitForVBlank();
	}

	this->MSData->DisplayWaitMsg("Checking UniStore...");
	
	/* Check if LastStore is accessible. */
	if (this->CData->LastStore() == "" || access((_STORE_PATH + this->CData->LastStore()).c_str(), F_OK) != 0) {
		if (access(_STORE_PATH "universal-db.unistore", F_OK) != 0) {
			if (Platform::WiFi::Connected()) {
				std::unique_ptr<Action> DL = std::make_unique<DownloadFile>(DEFAULT_UNISTORE, _STORE_PATH DEFAULT_UNISTORE_NAME);
				this->MSData->DisplayWaitMsg("Downloading " DEFAULT_UNISTORE_NAME "...");
				DL->Handler();

				DL = nullptr;
				DL = std::make_unique<DownloadFile>(DEFAULT_SPRITESHEET, _STORE_PATH DEFAULT_SPRITESHEET_NAME);
				this->MSData->DisplayWaitMsg("Downloading " DEFAULT_SPRITESHEET_NAME "...");
				DL->Handler();
			}
		}

		this->CData->LastStore(DEFAULT_UNISTORE_NAME);
	}

	this->Store = std::make_unique<UniStore>(this->CData->LastStore());

	this->_Tabs = std::make_unique<Tabs>();
	this->TGrid = std::make_unique<TopGrid>();
	this->TList = std::make_unique<TopList>();
	this->GData->UpdateUniStoreSprites();
};


/*
	Scan the key input.
*/
void UU::ScanInput() {
	Platform::ScanKeys();
	this->Down = Platform::KeysDown();
	this->Repeat = Platform::KeysDownRepeat();
	Platform::TouchRead(&this->T);
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
int UU::Handler(char *ARGV[]) {
	this->Initialize(ARGV);
	this->Draw(); // TODO: Maybe remove

	while(Platform::MainLoop() && !this->Exiting) {
		if(UU::App->Repeat) this->Draw(); // TODO: Draw every time
		Platform::waitForVBlank();
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
	
	Platform::Exit();

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

	this->GData->UpdateUniStoreSprites();
};