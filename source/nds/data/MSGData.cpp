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
#include "MSGData.hpp"


void MSGData::DisplayWaitMsg(const std::string &MSG) {
	/* Hide the Sprites on a message display. */
	if (UU::App->GData->SpritesDrawn) {
		UU::App->GData->SpritesDrawn = false;
		UU::App->GData->HideUniStoreSprites();
	}

	UU::App->GData->StartFrame();
	UU::App->GData->DrawTop();
	Gui::DrawStringCentered(0, 80, TEXT_MEDIUM, TEXT_COLOR, MSG);
	UU::App->GData->DrawBottom();
	UU::App->GData->EndFrame();
};


bool MSGData::PromptMsg(const std::string &MSG) {
	/* Hide the Sprites on a message display. */
	if (UU::App->GData->SpritesDrawn) {
		UU::App->GData->SpritesDrawn = false;
		UU::App->GData->HideUniStoreSprites();
	}

	bool Result = false;

	while(1) {
		UU::App->GData->StartFrame();
		UU::App->GData->DrawTop();
		Gui::DrawStringCentered(0, 80, TEXT_MEDIUM, TEXT_COLOR, MSG);
		Gui::DrawStringCentered(0, 210, TEXT_MEDIUM, TEXT_COLOR, "Press A to continue, B to cancel.");
		UU::App->GData->DrawBottom();
		UU::App->GData->EndFrame();

		do {
			swiWaitForVBlank();
			UU::App->ScanInput();
		} while (!(UU::App->Down & (KEY_A | KEY_B)));

		if (UU::App->Down & KEY_A) {
			Result = true;
			break;
		}

		if (UU::App->Down & KEY_B) {
			Result = false;
			break;
		}
	}

	UU::App->ScanInput();
	return Result;
};