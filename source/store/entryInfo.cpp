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

#include "storeUtils.hpp"
#include "structs.hpp"

/*
	Draw the Entry Info part.
*/
void StoreUtils::DrawEntryInfo(const std::unique_ptr<Store> &store, const std::unique_ptr<StoreEntry> &entry) {
	if (store && entry) { // Ensure, store & entry is not a nullptr.
		Gui::Draw_Rect(48, 0, 272, 36, C2D_Color32(50, 73, 98, 255));
		Gui::Draw_Rect(48, 36, 272, 1, C2D_Color32(25, 30, 53, 255));

		Gui::DrawStringCentered(25, 0, 0.6, C2D_Color32(255, 255, 255, 255), entry->GetTitle(), 265);
		Gui::DrawStringCentered(25, 20, 0.4, C2D_Color32(255, 255, 255, 255), entry->GetAuthor(), 265);
		Gui::DrawStringCentered(25, 50, 0.4, C2D_Color32(255, 255, 255, 255), entry->GetDescription(), 220);


		Gui::DrawString(61, 130, 0.45, C2D_Color32(255, 255, 255, 255), Lang::get("VERSION") + ": " + entry->GetVersion(), 265);
		Gui::DrawString(61, 145, 0.45, C2D_Color32(255, 255, 255, 255), Lang::get("CATEGORY") + ": " + entry->GetCategory(), 265);
		Gui::DrawString(61, 160, 0.45, C2D_Color32(255, 255, 255, 255), Lang::get("CONSOLE") + ": " + entry->GetConsole(), 265);
		Gui::DrawString(61, 175, 0.45, C2D_Color32(255, 255, 255, 255), Lang::get("LAST_UPDATED") + ": " + entry->GetLastUpdated(), 265);
		Gui::DrawString(61, 190, 0.45, C2D_Color32(255, 255, 255, 255), Lang::get("LICENSE") + ": " + entry->GetLicense(), 265);
		Gui::DrawString(61, 205, 0.45, C2D_Color32(255, 255, 255, 255), Lang::get("SIZE") + ": " + entry->GetSize(), 265);
	}
}

/*
	Entry Handle Logic.
*/
void StoreUtils::EntryHandle(u32 hDown, u32 hHeld, touchPosition touch, bool &showMark) {
	if (hDown & KEY_START) showMark = true;
}