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

#include <dirent.h>
#include <fat.h>
#include "nitrofs.h"
#include "UniversalUpdater.hpp"
#include "gui.hpp"
#include "tonccpy.h"


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
	Gui::init();
	this->SmallFont = std::make_unique<Font>(std::vector<std::string>({ "/_nds/Universal-Updater/font.nftr", "nitro:/graphics/font/test.nftr" }));

	constexpr uint16_t Palette[] = {
		0x0000, 0xB9CE, 0xD6B5, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0xB126, 0x9883, 0xA4A5, 0xFFFF, 0xCE0D, 0xC189, 0xF735
	};
	tonccpy(BG_PALETTE, Palette, sizeof(Palette));
	tonccpy(BG_PALETTE_SUB, Palette, sizeof(Palette));

	/* Load classes. */
	this->GData = std::make_unique<GFXData>();
	this->CData = std::make_unique<ConfigData>();
	this->TData = std::make_unique<ThemeData>();
	this->Store = std::make_unique<UniStore>("nitro:/test.unistore", "test.unistore");

	this->_Tabs = std::make_unique<Tabs>();
	this->TList = std::make_unique<TopList>();
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
	this->GData->DrawTop();

	/* Ensure it isn't a nullptr. */
	if (this->Store) {
		this->SmallFont->DrawString(0, 3, this->Store->GetUniStoreTitle(), Alignment::center);
		this->TList->Draw();
		this->_Tabs->DrawTop();

		this->SmallFont->DrawString(0, 30, "Title: " + this->Store->GetEntryTitle(0), Alignment::center);
		this->SmallFont->DrawString(0, 50, "Author: " + this->Store->GetEntryAuthor(0), Alignment::center);
		this->SmallFont->DrawString(0, 70, "Description: " + this->Store->GetEntryDescription(0), Alignment::center);
		this->SmallFont->DrawString(0, 90, "License: " + this->Store->GetEntryLicense(0), Alignment::center);
		this->SmallFont->DrawString(0, 110, "Index: " + std::to_string(0), Alignment::center);

	} else {
		this->SmallFont->DrawString(0, 3, "Invalid UniStore", Alignment::center);
	}

	this->SmallFont->update(true);

	this->GData->DrawBottom();
	this->_Tabs->DrawBottom();

	this->SmallFont->update(false);
};


/*
	Main Handler of the app. Handle Input and display stuff here.
*/
int UU::Handler(char *ARGV[]) {
	this->Initialize(ARGV);

	while (!this->Exiting) {
		swiWaitForVBlank();

		this->Draw();
		this->ScanInput();

		/* Handle Top List if possible. */
		if (this->_Tabs->HandleTopScroll()) this->TList->Handler();
		this->_Tabs->Handler(); // Tabs are always handled.
	}

	this->CData->Sav();
	return 0;
};


bool UU::Touched(const Structs::ButtonPos Pos) const {
	return ((this->T.px >= Pos.x && this->T.px <= (Pos.x + Pos.w)) && (this->T.py >= Pos.y && this->T.py <= (Pos.y + Pos.h)));
};