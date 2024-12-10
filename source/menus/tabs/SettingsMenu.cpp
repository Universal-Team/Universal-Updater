// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "SettingsMenu.hpp"

#include "gui.hpp"
#include "QueueSystem.hpp"
#include "UniStoreSelector.hpp"
#include "UniversalUpdater.hpp"
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
				/* Don't open while Queue is not empty. */
				if (QueueSystem::Count() == 0) {
					UU::App->USelector->InitSelector();
					UU::App->Draw();
				}
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