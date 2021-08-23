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
	hidSetRepeatParameters(20, 8);

	this->GData = std::make_unique<GFXData>();
	this->Store = std::make_unique<UniStore>("romfs:/test.unistore", "test.unistore");

	this->_Tabs = std::make_unique<Tabs>();
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
	Gui::clearTextBufs();
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	this->GData->DrawTop();

	Gui::DrawStringCentered(0, 3, 0.5f, TEXT_COLOR, "Universal-Updater");
	Gui::DrawStringCentered(0, 30, 0.45f, TEXT_COLOR, "Title: " + this->Store->GetEntryTitle(0));
	Gui::DrawStringCentered(0, 50, 0.45f, TEXT_COLOR, "Author: " + this->Store->GetEntryAuthor(0));
	Gui::DrawStringCentered(0, 70, 0.45f, TEXT_COLOR, "Description: " + this->Store->GetEntryDescription(0));
	Gui::DrawStringCentered(0, 90, 0.45f, TEXT_COLOR, "License: " + this->Store->GetEntryLicense(0));
	Gui::DrawStringCentered(0, 110, 0.45f, TEXT_COLOR, "Index: " + std::to_string(0));

	this->GData->DrawBottom();
	this->_Tabs->Draw();

	C3D_FrameEnd(0);
};


/*
	Main Handler of the app. Handle Input and display stuff here.
*/
int UU::Handler() {
	this->Initialize();

	while(aptMainLoop() && !this->Exiting) {
		this->Draw();
		this->ScanInput();
		this->_Tabs->Handler();
	}

	Gui::exit();
	gfxExit();
	romfsExit();

	return 0;
};


bool UU::Touched(const Structs::ButtonPos Pos) const {
	return ((this->T.px >= Pos.x && this->T.px <= (Pos.x + Pos.w)) && (this->T.py >= Pos.y && this->T.py <= (Pos.y + Pos.h)));
};