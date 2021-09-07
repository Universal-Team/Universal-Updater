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

#include "Common.hpp"
#include "SettingsMenu.hpp"
#include "UniStoreSelector.hpp"
#include "Utils.hpp"


void SettingsMenu::Draw() {
	switch(this->_Menu) {
		case Menu::Main:
			this->DrawMain();
			break;
	}
};


void SettingsMenu::Handler() {
	switch(this->_Menu) {
		case Menu::Main:
			this->HandleMain();
			break;
	}
};


void SettingsMenu::DrawMain() {
	/* Draw Top bar. */
	Gui::Draw_Rect(40, 0, 280, 25, BAR_COLOR);
	Gui::Draw_Rect(40, 25, 280, 1, BAR_OUTLINE);
	Gui::DrawStringCentered(17, 2, TEXT_MEDIUM, TEXT_COLOR, "Settings", 273, 0);

	for (uint8_t Idx = 0; Idx < 2; Idx++) {
		if (Idx == this->Selection) Gui::Draw_Rect(this->MainPos[Idx].x, this->MainPos[Idx].y, this->MainPos[Idx].w, this->MainPos[Idx].h, TABS_SELECTED);
		Gui::DrawStringCentered(20, this->MainPos[Idx].y + 4, TEXT_SMALL, TEXT_COLOR, this->MainStrings[Idx], 255, 0);
	}
};


void SettingsMenu::HandleMain() {
	if (UU::App->Down & KEY_A) {
		switch(this->Selection) {
			case 0:
				UU::App->USelector->InitSelector();
				UU::App->Draw();
				break;

			case 1:
				UU::App->Exiting = true;
				break;
		}
	}

	if (UU::App->Repeat & KEY_DOWN) {
		if (this->Selection < 1) this->Selection++;
		else this->Selection = 0;
	}

	if (UU::App->Repeat & KEY_UP) {
		if (this->Selection > 0) this->Selection--;
		else this->Selection = 1;
	}
};