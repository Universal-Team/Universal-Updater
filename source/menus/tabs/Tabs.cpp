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

#include "Tabs.hpp"

#include "gui.hpp"
#include "QueueSystem.hpp"
#include "UniversalUpdater.hpp"


/* Init Tab components. */
Tabs::Tabs() {
	this->EInfo = std::make_unique<EntryInfo>();
	this->DList = std::make_unique<DownList>();
	this->QMenu = std::make_unique<QueueMenu>();
	this->SeMenu = std::make_unique<SearchMenu>();
	this->SoMenu = std::make_unique<SortMenu>();
	this->SMenu = std::make_unique<SettingsMenu>();
};


/*
	Mainly used for the Download List, because it has an overlay for the top screen.
*/
void Tabs::DrawTop() {
	if (this->ActiveTab == Tabs::Tab::DownloadList) this->DList->DrawTopOvl();
};


void Tabs::DrawBottom() {
	for (uint8_t Idx = 0; Idx < 6; Idx++) {
		if (Idx == (uint8_t)this->ActiveTab) Gui::Draw_Rect(this->TabPos[Idx].x, this->TabPos[Idx].y, this->TabPos[Idx].w, this->TabPos[Idx].h, TABS_SELECTED);
		else Gui::Draw_Rect(this->TabPos[Idx].x, this->TabPos[Idx].y, this->TabPos[Idx].w, this->TabPos[Idx].h, TABS_UNSELECTED);
	}

	UU::App->GData->DrawSpriteBlend(sprites_info_idx, this->TabPos[0].x, this->TabPos[0].y);
	UU::App->GData->DrawSpriteBlend(sprites_download_idx, this->TabPos[1].x, this->TabPos[1].y);
	UU::App->GData->DrawSpriteBlend(sprites_queue0_idx, this->TabPos[2].x, this->TabPos[2].y);
	UU::App->GData->DrawSpriteBlend(sprites_search_idx, this->TabPos[3].x, this->TabPos[3].y);
	UU::App->GData->DrawSpriteBlend(sprites_sort_idx, this->TabPos[4].x, this->TabPos[4].y);
	UU::App->GData->DrawSpriteBlend(sprites_settings_idx, this->TabPos[5].x, this->TabPos[5].y);

	Gui::DrawStringCentered(this->TabPos[2].x + this->TabPos[2].w / 2 - 160, this->TabPos[2].y + (this->TabPos[2].h - Gui::GetStringHeight(TEXT_MEDIUM, "0")) / 2, TEXT_MEDIUM, TAB_ICON_COLOR, std::to_string(QueueSystem::Count()));

	/* Draw Active Tab. */
	switch(this->ActiveTab) {
		case Tab::EntryInfo:
			this->EInfo->Draw();
			break;

		case Tab::DownloadList:
			this->DList->Draw();
			break;

		case Tab::QueueSystem:
			this->QMenu->Draw();
			break;

		case Tab::Search:
			this->SeMenu->Draw();
			break;

		case Tab::Sort:
			this->SoMenu->Draw();
			break;

		case Tab::Settings:
			this->SMenu->Draw();
			break;
	}

	// Gui::Draw_Rect(40, 0, 1, 240, BAR_OUTLINE);
};


void Tabs::Handler() {
	/* Handle active tab. */
	switch(this->ActiveTab) {
		case Tab::EntryInfo:
			this->EInfo->Handler();
			break;

		case Tab::DownloadList:
			this->DList->Handler();
			break;
			
		case Tab::QueueSystem:
			this->QMenu->Handler();
			break;

		case Tab::Search:
			this->SeMenu->Handler();
			break;

		case Tab::Sort:
			this->SoMenu->Handler();
			break;

		case Tab::Settings:
			this->SMenu->Handler();
			break;
	}
	
	/* Tab Switch Handler. */
	if (UU::App->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 6; Idx++) {
			if (this->TabPos[Idx].Touched(UU::App->T)) {
				this->SwitchTab((Tabs::Tab)Idx);
				break;
			}
		}
	}

	if (UU::App->Repeat & KEY_L) {
		if (this->ActiveTab != Tab::EntryInfo) {
			const uint8_t T = (uint8_t)this->ActiveTab - 1;
			this->SwitchTab((Tabs::Tab)T);
		}
	}
	
	if (UU::App->Repeat & KEY_R) {
		if (this->ActiveTab != Tab::Settings) {
			const uint8_t T = (uint8_t)this->ActiveTab + 1;
			this->SwitchTab((Tabs::Tab)T);
		}
	}
};


void Tabs::PrevTab() {
	if (this->LastTab == Tabs::Tab::DownloadList) this->DList->Reset();
	const Tabs::Tab T = this->ActiveTab;

	this->ActiveTab = this->LastTab;
	this->LastTab = T;
};


void Tabs::SwitchTab(const Tabs::Tab T) {
	if (T == Tabs::Tab::DownloadList) this->DList->Reset();
	this->LastTab = this->ActiveTab;
	this->ActiveTab = T;
};