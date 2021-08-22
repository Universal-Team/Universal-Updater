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


/*
	Initialize everything as needed.
*/
void UU::Initialize() {
	romfsInit();
	gfxInitDefault();
	Gui::init();

	this->Store = std::make_unique<UniStore>("romfs:/test.unistore", "test.unistore");
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
	
};


/*
	Main Handler of the app. Handle Input and display stuff here.
*/
int UU::Handler() {
	this->Initialize();

	while(aptMainLoop() && !this->Exiting) {

	}

	Gui::exit();
	gfxExit();
	romfsExit();

	return 0;
};