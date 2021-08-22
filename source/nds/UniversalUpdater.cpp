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


/*
	Initialize everything as needed.
*/
void UU::Initialize(char *ARGV[]) {
	consoleDemoInit();
	fatInitDefault();
	
	if (nitroFSInit(ARGV[0])) {
		this->Store = std::make_unique<UniStore>("nitro:/test.unistore", "test.unistore");
	}
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
		iprintf("Title: %s\n", this->Store->GetEntryTitle(0).c_str());
		iprintf("Author: %s\n", this->Store->GetEntryAuthor(0).c_str());
		iprintf("Description: %s\n", this->Store->GetEntryDescription(0).c_str());
		iprintf("License: %s\n", this->Store->GetEntryLicense(0).c_str());
		iprintf("Index: %d\n", 0);
	}

	while(!this->Exiting) {
		swiWaitForVBlank();
	}

	return 0;
};