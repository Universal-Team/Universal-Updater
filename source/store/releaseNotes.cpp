/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 Universal-Team
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

#include "download.hpp"
#include "storeUtils.hpp"

void StoreUtils::DrawReleaseNotes(const int &scrollIndex, const std::unique_ptr<StoreEntry> &entry, const std::unique_ptr<Store> &store) {
	if (entry && store) {
		Gui::ScreenDraw(Top);
		Gui::Draw_Rect(0, 26, 400, 214, BG_COLOR);
		Gui::DrawString(5, 25 - scrollIndex, 0.5f, TEXT_COLOR, entry->GetReleaseNotes(), 390, 0, font, C2D_WordWrap);
		Gui::Draw_Rect(0, 0, 400, 25, BAR_COLOR);
		Gui::Draw_Rect(0, 25, 400, 1, BAR_OUTL_COLOR);
		Gui::DrawStringCentered(0, 1, 0.7f, TEXT_COLOR, entry->GetTitle(), 390, 0, font);

	} else {
		Gui::ScreenDraw(Top);
		Gui::Draw_Rect(0, 0, 400, 25, BAR_COLOR);
		Gui::Draw_Rect(0, 25, 400, 1, BAR_OUTL_COLOR);
		Gui::Draw_Rect(0, 26, 400, 214, BG_COLOR);
	}
}

/*
	As the name says: Release notes logic.

	int &scrollIndex: The scroll index for the Release Notes text.
	int &storeMode: The store mode to properly return back.
*/
void StoreUtils::ReleaseNotesLogic(int &scrollIndex, int &storeMode) {
	if (hRepeat & KEY_DOWN) scrollIndex += Gui::GetStringHeight(0.5f, "", font);

	if (hRepeat & KEY_UP) {
		if (scrollIndex > 0) scrollIndex -= Gui::GetStringHeight(0.5f, "", font);
	}

	if (hDown & KEY_B) {
		scrollIndex = 0;
		storeMode = 0;
	}
}


/*
	I place it temporarely here for now.

	Display Release changelog for Universal-Updater.
*/
void DisplayChangelog() {
	if (config->changelog()) {
		config->changelog(false);

		bool confirmed = false;
		const std::string notes = GetChangelog();
		if (notes == "") return;
		int scrollIndex = 0;

		while(!confirmed) {
			Gui::clearTextBufs();
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
			C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));

			Gui::ScreenDraw(Top);
			Gui::Draw_Rect(0, 26, 400, 214, BG_COLOR);
			Gui::DrawString(5, 25 - scrollIndex, 0.5f, TEXT_COLOR, notes, 390, 0, font, C2D_WordWrap);
			Gui::Draw_Rect(0, 0, 400, 25, BAR_COLOR);
			Gui::Draw_Rect(0, 25, 400, 1, BAR_OUTL_COLOR);
			Gui::DrawStringCentered(0, 1, 0.7f, TEXT_COLOR, "Universal-Updater", 390, 0, font);
			Gui::Draw_Rect(0, 215, 400, 25, BAR_COLOR);
			Gui::Draw_Rect(0, 214, 400, 1, BAR_OUTL_COLOR);
			Gui::DrawStringCentered(0, 217, 0.7f, TEXT_COLOR, C_V, 390, 0, font);

			GFX::DrawBottom();
			Gui::Draw_Rect(0, 0, 320, 25, BAR_COLOR);
			Gui::Draw_Rect(0, 25, 320, 1, BAR_OUTL_COLOR);
			C3D_FrameEnd(0);

			hidScanInput();
			touchPosition t;
			touchRead(&t);
			u32 repeat = hidKeysDownRepeat();
			u32 down = hidKeysDown();

			/* Scroll Logic. */
			if (repeat & KEY_DOWN) scrollIndex += Gui::GetStringHeight(0.5f, "", font);

			if (repeat & KEY_UP) {
				if (scrollIndex > 0) scrollIndex -= Gui::GetStringHeight(0.5f, "", font);
			}

			if ((down & KEY_A) || (down & KEY_B) || (down & KEY_START) || (down & KEY_TOUCH)) confirmed = true;
		}
	}
}