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
#include "nitrofs.h"
#include "QueueSystem.hpp"
#include "tonccpy.h"
#include "WiFi.hpp"

#include <dirent.h>
#include <fat.h>
#include <unistd.h>


/*
	Initialize everything as needed.
*/
void UU::Initialize(char *ARGV[]) {
	keysSetRepeat(20, 8);

	if (!fatInitDefault()) {
		consoleDemoInit();
		iprintf("FAT init failed!\n");
		while (1) swiWaitForVBlank();
	}

	/* Try init NitroFS at a few likely locations. */
	if (!nitroFSInit(ARGV[0])) {
		if (!nitroFSInit("Universal-Updater.nds")) {
			if (!nitroFSInit("/_nds/Universal-Updater/Universal-Updater.nds")) {
				consoleDemoInit();
				iprintf("NitroFS init failed!\n\n");
				iprintf("Copy Universal-Updater.nds to\n\n");
				iprintf("/_nds/Universal-Updater/\n");
				iprintf("           Universal-Updater.nds\n");
				iprintf("or launch using TWiLight Menu++\nor nds-hb-menu.");
				while (1) swiWaitForVBlank();
			}
		}
	}
	
	/* Create Directories. */
	mkdir("/_nds", 0x777);
	mkdir("/_nds/Universal-Updater", 0x777);
	mkdir("/_nds/Universal-Updater/stores", 0x777);
	mkdir("/_nds/Universal-Updater/shortcuts", 0x777);

	/* Initialize graphics. */
	Gui::init({"sd:/_nds/Universal-Updater/font.nftr", "fat:/_nds/Universal-Updater/font.nftr", "nitro:/graphics/font/default.nftr"});

	constexpr uint16_t Palette[] = {
		0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xB126, 0xB126, 0xB126, 0xB126, 0x9883, 0x9883, 0x9883, 0x9883, 0xA4A5, 0xA4A5, 0xA4A5, 0xA4A5,
		0xFFFF, 0xB9CE, 0xD6B5, 0xFFFF, 0xCE0D, 0xCE0D, 0xCE0D, 0xCE0D, 0xC189, 0xC189, 0xC189, 0xC189, 0xF735, 0xC1CC, 0xD22E, 0xF735
	};
	tonccpy(BG_PALETTE, Palette, sizeof(Palette));
	tonccpy(BG_PALETTE_SUB, Palette, sizeof(Palette));

	this->GData = std::make_unique<GFXData>();
	this->MSData = std::make_unique<MSGData>();

	/* Initialize Wi-Fi if not using no$gba. */
	if (strncmp((const char *)0x4FFFA00, "no$gba", 6) != 0) {
		this->GData->StartFrame();
		this->GData->DrawTop();
		Gui::DrawStringCentered(0, 3, TEXT_LARGE, TEXT_COLOR, "Connecting to Wi-Fi...");
		Gui::DrawStringCentered(0, 30, TEXT_MEDIUM, TEXT_COLOR, "If it doesn't connect, your router\nmay be incompatible at the moment.");
		this->GData->DrawBottom();
		this->GData->EndFrame();

		WiFi::Init();
		while(!WiFi::Connected()) {
			swiWaitForVBlank();
		}
	}

	/* Load classes. */
	this->CData = std::make_unique<ConfigData>();
	this->TData = std::make_unique<ThemeData>();
	this->MData = std::make_unique<Meta>();

	this->MSData->DisplayWaitMsg("Checking UniStore...");

	/* Check if LastStore is accessible. */
	if (this->CData->LastStore() == "" || access((_STORE_PATH + this->CData->LastStore()).c_str(), F_OK) != 0) {
		if (access(_STORE_PATH "universal-db.unistore", F_OK) != 0) {
			if (DownloadUtils::WiFiAvailable()) {
				std::unique_ptr<Action> DL = std::make_unique<DownloadFile>("https://github.com/Universal-Team/db/raw/master/docs/unistore/universal-db.unistore", _STORE_PATH "universal-db.unistore");
				this->MSData->DisplayWaitMsg("Downloading universal-db.unistore...");
				DL->Handler();

				DL = nullptr;
				DL = std::make_unique<DownloadFile>("https://github.com/Universal-Team/db/raw/master/docs/unistore/universal-db.tdx", _STORE_PATH "universal-db.tdx");
				this->MSData->DisplayWaitMsg("Downloading universal-db.tdx...");
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
	this->GData->UpdateUniStoreSprites();
};


/*
	Scan the key input.
*/
void UU::ScanInput() {
	scanKeys();
	this->Down = keysDown();
	this->Repeat = keysDownRepeat();
	touchRead(&this->T);
	this->T.px = this->T.px * 5 / 4;
	this->T.py = this->T.py * 5 / 4;
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

void UU::VBlankHandler() {
	UU::App->ScanInput();

	UU::App->_Tabs->Handler(); // Tabs are always handled.

	/* Handle Top List if possible. */
	if (UU::App->_Tabs->HandleTopScroll()) {
		switch(UU::App->TMode) {
			case TopMode::Grid:
				UU::App->TGrid->Handler();
				break;

			case TopMode::List:
				UU::App->TList->Handler();
				break;
		}
	}

	if (UU::App->Repeat) UU::App->Draw();
}


/*
	Main Handler of the app. Handle Input and display stuff here.
*/
int UU::Handler(char *ARGV[]) {
	this->Initialize(ARGV);

	// TODO: It only redraws on button press, forcing an initial draw for now
	this->Draw();

	/* Enable VBlank handler. */
	irqSet(IRQ_VBLANK, this->VBlankHandler);
	irqEnable(IRQ_VBLANK);

	while (!this->Exiting) {
		swiWaitForVBlank();

		QueueSystem::Handler();
	}

	this->CData->Sav();
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