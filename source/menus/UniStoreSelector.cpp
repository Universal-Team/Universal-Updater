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
#include "UniStoreSelector.hpp"
#include "Utils.hpp"
#include <unistd.h>


/*
	Delete a store.. including the Spritesheets, if found.

	const std::string &File: The filename of the UniStore.
*/
void UniStoreSelector::DeleteUniStore(const std::string &File) {
	nlohmann::json StoreJSON;

	FILE *Tmp = fopen((std::string(_STORE_PATH) + File).c_str(), "rt");

	if (Tmp) {
		StoreJSON = nlohmann::json::parse(Tmp, nullptr, false);
		fclose(Tmp);
	}

	if (StoreJSON.is_discarded()) StoreJSON = { };

	/* Check, if Spritesheet exist on UniStore. */
	if (StoreJSON["storeInfo"].contains(SHEET_PATH_KEY) && StoreJSON["storeInfo"][SHEET_PATH_KEY].is_array()) {
		const std::vector<std::string> Sheet = StoreJSON["storeInfo"][SHEET_PATH_KEY].get<std::vector<std::string>>();

		/* Cause it's an array, delete all Spritesheets which exist. */
		for (size_t Idx = 0; Idx < Sheet.size(); Idx++) {
			if (Sheet[Idx] != "") {
				if (!(Utils::LowerCase(Sheet[Idx]).find(Utils::LowerCase("/")) != std::string::npos)) {
					if (access((std::string(_STORE_PATH) + Sheet[Idx]).c_str(), F_OK) == 0) {
						std::remove((std::string(_STORE_PATH) + Sheet[Idx]).c_str());
					}
				}
			}
		}

	/* Else, if it's just a string.. check and delete single Spritesheet. */
	} else if (StoreJSON["storeInfo"].contains(SHEET_PATH_KEY) && StoreJSON["storeInfo"][SHEET_PATH_KEY].is_string()) {
		const std::string Sheet = StoreJSON["storeInfo"][SHEET_PATH_KEY];

		if (Sheet != "") {
			if (!(Utils::LowerCase(Sheet).find(Utils::LowerCase("/")) != std::string::npos)) {
				if (access((std::string(_STORE_PATH) + Sheet).c_str(), F_OK) == 0) {
					std::remove((std::string(_STORE_PATH) + Sheet).c_str());
				}
			}
		}
	}

	std::remove((std::string(_STORE_PATH) + File).c_str()); // Now delete UniStore.
};


/*
	Download a UniStore.. including the SpriteSheets, if found.

	TODO: Proper idea of handling.
*/
void UniStoreSelector::DownloadUniStore() {

};


/*
	Update a UniStore.. including the SpriteSheets, if found.

	TODO: Proper idea of handling.
*/
void UniStoreSelector::UpdateUniStore() {

};



void UniStoreSelector::Handler() {
	this->Infos = UniStore::GetUniStoreInfo(_STORE_PATH);
	bool HasDrawn = false;

	#ifdef ARM9
		UU::App->GData->HideUniStoreSprites();
	#endif


	while(!this->Done) {
		#ifdef ARM9
			swiWaitForVBlank();
			if (HasDrawn) goto Logic; // If already drawn -> Just do the logic.
			HasDrawn = true;
		#endif

		UU::App->GData->StartFrame();
		UU::App->GData->DrawTop();

		if (!this->Infos.empty()) {
			if (this->Infos[this->SelectedIndex].StoreSize != -1) {
				Gui::DrawStringCentered(0, 1, TEXT_LARGE, TEXT_COLOR, this->Infos[this->SelectedIndex].Title, 390, 0);
				Gui::DrawStringCentered(0, 30, TEXT_LARGE, TEXT_COLOR, this->Infos[this->SelectedIndex].Author, 380, 0);
				Gui::DrawStringCentered(0, 70, TEXT_MEDIUM, TEXT_COLOR, this->Infos[this->SelectedIndex].Description, 380, 130, nullptr, C2D_WordWrap);

			} else {
				Gui::DrawStringCentered(0, 1, TEXT_LARGE, TEXT_COLOR, "Invalid UniStore", 390, 0);
			}

			Gui::DrawString(10, 190, TEXT_SMALL, TEXT_COLOR, "- Entries: " + std::to_string(this->Infos[this->SelectedIndex].StoreSize), 150, 0);
			Gui::DrawString(10, 200, TEXT_SMALL, TEXT_COLOR, "- Version: " + std::to_string(this->Infos[this->SelectedIndex].Version), 150, 0);
			Gui::DrawString(10, 210, TEXT_SMALL, TEXT_COLOR, "- Revision: " + std::to_string(this->Infos[this->SelectedIndex].Revision), 150, 0);

			UU::App->GData->DrawBottom();

			Gui::Draw_Rect(0, 0, 320, 25, BAR_COLOR);
			Gui::Draw_Rect(0, 25, 320, 1, BAR_OUTLINE);
			UU::App->GData->DrawSpriteBlend(sprites_arrow_idx, this->Pos[9].x, this->Pos[9].y, TEXT_COLOR);
			Gui::DrawStringCentered(0, 2, TEXT_LARGE, TEXT_COLOR, "Select a UniStore", 310, 0);

			for(size_t Idx = 0; Idx < 6 && Idx < this->Infos.size(); Idx++) {
				if (this->ScreenIndex + Idx == this->SelectedIndex) Gui::Draw_Rect(this->Pos[Idx].x, this->Pos[Idx].y, this->Pos[Idx].w, this->Pos[Idx].h, BAR_COLOR);
				Gui::DrawStringCentered(10 - 160 + (300 / 2), this->Pos[Idx].y + 4, TEXT_SMALL, TEXT_COLOR, this->Infos[this->ScreenIndex + Idx].FileName, 295, 0);
			}

		} else {
			UU::App->GData->DrawBottom();
		}

		UU::App->GData->DrawSpriteBlend(sprites_delete_idx, this->Pos[6].x, this->Pos[6].y, TEXT_COLOR);
		UU::App->GData->DrawSpriteBlend(sprites_update_idx, this->Pos[7].x, this->Pos[7].y, TEXT_COLOR);
		UU::App->GData->DrawSpriteBlend(sprites_add_idx, this->Pos[8].x, this->Pos[8].y, TEXT_COLOR);
		UU::App->GData->EndFrame();

		#ifdef ARM9
			Logic:
		#endif

			UU::App->ScanInput();

			if (!this->Infos.empty()) {
				if (UU::App->Repeat & KEY_DOWN) {
					if (this->SelectedIndex < this->Infos.size() - 1) this->SelectedIndex++;
					else this->SelectedIndex = 0;
					HasDrawn = false;
				}

				if (UU::App->Repeat & KEY_UP) {
					if (this->SelectedIndex > 0) this->SelectedIndex--;
					else this->SelectedIndex = this->Infos.size() - 1;
					HasDrawn = false;
				}

				if (UU::App->Repeat & KEY_RIGHT) {
					if (this->SelectedIndex + 6 < this->Infos.size() - 1) this->SelectedIndex += 6;
					else this->SelectedIndex = this->Infos.size() - 1;
					HasDrawn = false;
				}

				if (UU::App->Repeat & KEY_LEFT) {
					if (this->SelectedIndex >= 6) this->SelectedIndex -= 6;
					else this->SelectedIndex = 0;
					HasDrawn = false;
				}

				if (UU::App->Down & KEY_A) {
					if (this->Infos[this->SelectedIndex].File != "") { // Ensure to check for this.
						if (!(this->Infos[this->SelectedIndex].File.find("/") != std::string::npos)) {
							/* Load selected one. */
							if (this->Infos[this->SelectedIndex].Version == -1) UU::App->MSData->PromptMsg("UniStore invalid!");
							else if (this->Infos[this->SelectedIndex].Version < 3) UU::App->MSData->PromptMsg("UniStore too old!");
							else if (this->Infos[this->SelectedIndex].Version > UU::App->Store->UNISTORE_VERSION) UU::App->MSData->PromptMsg("UniStore too new!");
							else {
								UU::App->Store = nullptr; // Needs to be set to nullptr first for some reason to properly load the Icons...
								UU::App->Store = std::make_unique<UniStore>(_STORE_PATH + this->Infos[this->SelectedIndex].FileName, this->Infos[this->SelectedIndex].FileName);
								UU::App->_Tabs->SortEntries();
								UU::App->SwitchTopMode(UU::App->TMode);
								UU::App->CData->LastStore(this->Infos[this->SelectedIndex].FileName);
								this->Done = true;
							}

						} else {
							UU::App->MSData->PromptMsg("UniStore contains a slash and hence is invalid.");
						}
					}
				}

				if (UU::App->Down & KEY_TOUCH) {
					for (uint8_t Idx = 0; Idx < 6; Idx++) {
						if (this->Pos[Idx].Touched(UU::App->T)) {
							if (Idx + this->ScreenIndex < this->Infos.size() && this->Infos[Idx + this->ScreenIndex].File != "") { // Ensure to check for this.
								if (!(this->Infos[Idx + this->ScreenIndex].File.find("/") != std::string::npos)) {
									if (this->Infos[Idx + this->ScreenIndex].Version == -1) UU::App->MSData->PromptMsg("UniStore invalid!");
									else if (this->Infos[Idx + this->ScreenIndex].Version < 3) UU::App->MSData->PromptMsg("UniStore too old!");
									else if (this->Infos[Idx + this->ScreenIndex].Version > UU::App->Store->UNISTORE_VERSION) UU::App->MSData->PromptMsg("UniStore too new!");
									else {
										UU::App->Store = nullptr; // Needs to be set to nullptr first for some reason to properly load the Icons...
										UU::App->Store = std::make_unique<UniStore>(_STORE_PATH + this->Infos[Idx + this->ScreenIndex].FileName, this->Infos[Idx + this->ScreenIndex].FileName);
										UU::App->_Tabs->SortEntries();
										UU::App->SwitchTopMode(UU::App->TMode);
										UU::App->CData->LastStore(this->Infos[Idx + this->ScreenIndex].FileName);
										this->Done = true;
									}

								} else {
									UU::App->MSData->PromptMsg("UniStore contains a slash and hence is invalid.");
								}
							}
						}
					}
				}

				/* Delete UniStore. For now comment out. */
				if ((UU::App->Down & KEY_X) || (UU::App->Down & KEY_TOUCH && this->Pos[6].Touched(UU::App->T))) {
					if (this->Infos[this->SelectedIndex].FileName != "") {
						this->DeleteUniStore(this->Infos[this->SelectedIndex].FileName);
						this->SelectedIndex = 0;
						this->Infos = UU::App->Store->GetUniStoreInfo(_STORE_PATH);
						HasDrawn = false;
					}
				}

				/* Download latest UniStore. */
				if ((UU::App->Down & KEY_START) || (UU::App->Down & KEY_TOUCH && this->Pos[7].Touched(UU::App->T))) {

				}

				if (this->SelectedIndex < this->ScreenIndex) this->ScreenIndex = this->SelectedIndex;
				else if (this->SelectedIndex > this->ScreenIndex + 6 - 1) this->ScreenIndex = this->SelectedIndex - 6 + 1;
			}

			/* UniStore QR Code / URL Download. */
			if ((UU::App->Down & KEY_Y) || (UU::App->Down & KEY_TOUCH && this->Pos[8].Touched(UU::App->T))) {

			}

			/* Go out of the menu. */
			if ((UU::App->Down & KEY_B) || (UU::App->Down & KEY_TOUCH && this->Pos[9].Touched(UU::App->T))) this->Done = true;
	}

	#ifdef ARM9
		UU::App->GData->UpdateUniStoreSprites();
	#endif
};