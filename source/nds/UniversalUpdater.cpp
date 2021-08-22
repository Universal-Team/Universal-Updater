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

#include <fat.h>
#include "nitrofs.h"
#include "UniversalUpdater.hpp"
#include "graphics.hpp"
#include "tonccpy.h"

Font *UU::font;

/*
	Initialize everything as needed.
*/
void UU::Initialize(char *ARGV[]) {
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
	
	/* Initialize graphics. */
	Graphics::init();
	font = new Font({std::string("/_nds/Universal-Updater/font.nftr"), "nitro:/graphics/font/test.nftr"});

	u16 palette[] = {
		0x0000,
		0xB9CE,
		0xD6B5,
		0xFFFF,
	};
	tonccpy(BG_PALETTE, palette, sizeof(palette));
	tonccpy(BG_PALETTE_SUB, palette, sizeof(palette));

	/* Load UniStore. */
	this->Store = std::make_unique<UniStore>("nitro:/test.unistore", "test.unistore");
};


/*
	Scan the key input.
*/
void UU::ScanInput() {
	scanKeys();
	this->Down = keysDown();
	this->Repeat = keysDownRepeat();
	touchRead(&this->T);
};


/*
	Draws Universal-Updater's UI.
*/
void UU::Draw() {
	
};


/*
	Main Handler of the app. Handle Input and display stuff here.
*/
int UU::Handler(char *ARGV[]) {
	this->Initialize(ARGV);

	if (this->Store) {
		this->font->print("Title: " + this->Store->GetEntryTitle(0), 0, font->height() * 0, false);
		this->font->print("Author: " + this->Store->GetEntryAuthor(0), 0, font->height() * 1, false);
		this->font->print("Description: " + this->Store->GetEntryDescription(0), 0, font->height() * 2, false);
		this->font->print("License: " + this->Store->GetEntryLicense(0), 0, font->height() * 3, false);
		this->font->print("Index: " + std::to_string(0), 0, font->height() * 4, false);
	}

	this->font->update();

	while(!this->Exiting) {
		swiWaitForVBlank();
	}

	return 0;
};