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

extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<Structs::ButtonPos> markBox = {
	{10, 94, 52, 52},
	{72, 94, 52, 52},
	{134, 94, 52, 52},
	{196, 94, 52, 52},
	{258, 94, 52, 52}
};

/*
	Draw the Entry Info part.
*/
void StoreUtils::DisplayMarkBox(const int &marks) {
	Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(0, 0, 0, 190)); // Darken.

	GFX::drawBox(markBox[0].x, markBox[0].y, markBox[0].w, markBox[0].h, marks & favoriteMarks::STAR);
	GFX::drawBox(markBox[1].x, markBox[1].y, markBox[1].w, markBox[1].h, marks & favoriteMarks::HEART);
	GFX::drawBox(markBox[2].x, markBox[2].y, markBox[2].w, markBox[2].h, marks & favoriteMarks::DIAMOND);
	GFX::drawBox(markBox[3].x, markBox[3].y, markBox[3].w, markBox[3].h, marks & favoriteMarks::CLUBS);
	GFX::drawBox(markBox[4].x, markBox[4].y, markBox[4].w, markBox[4].h, marks & favoriteMarks::SPADE);

	Gui::DrawString(markBox[0].x + 15, markBox[0].y + 12, 0.9, C2D_Color32(255, 255, 255, 255), "★");
	Gui::DrawString(markBox[1].x + 15, markBox[1].y + 12, 0.9, C2D_Color32(255, 255, 255, 255), "♥");
	Gui::DrawString(markBox[2].x + 15, markBox[2].y + 12, 0.9, C2D_Color32(255, 255, 255, 255), "♦");
	Gui::DrawString(markBox[3].x + 15, markBox[3].y + 12, 0.9, C2D_Color32(255, 255, 255, 255), "♣");
	Gui::DrawString(markBox[4].x + 15, markBox[4].y + 12, 0.9, C2D_Color32(255, 255, 255, 255), "♠");
}

/*
	Mark Menu handle.
*/
void StoreUtils::MarkHandle(u32 hDown, u32 hHeld, touchPosition touch, std::unique_ptr<StoreEntry> &entry, const std::unique_ptr<Store> &store, bool &showMark, std::unique_ptr<Meta> &meta) {
	if (meta && entry) {
		if (hDown & KEY_TOUCH) {
			/* Star. */
			if (touching(touch, markBox[0])) {
				meta->SetMarks(store->GetUniStoreTitle(), entry->GetTitle(),
					meta->GetMarks(store->GetUniStoreTitle(), entry->GetTitle()) ^ favoriteMarks::STAR);
				entry->SetMark(meta->GetMarks(store->GetUniStoreTitle(), entry->GetTitle()));

			/* Heart. */
			} else if (touching(touch, markBox[1])) {
				meta->SetMarks(store->GetUniStoreTitle(), entry->GetTitle(),
					meta->GetMarks(store->GetUniStoreTitle(), entry->GetTitle()) ^ favoriteMarks::HEART);
				entry->SetMark(meta->GetMarks(store->GetUniStoreTitle(), entry->GetTitle()));

			/* Diamond. */
			} else if (touching(touch, markBox[2])) {
				meta->SetMarks(store->GetUniStoreTitle(), entry->GetTitle(),
					meta->GetMarks(store->GetUniStoreTitle(), entry->GetTitle()) ^ favoriteMarks::DIAMOND);
				entry->SetMark(meta->GetMarks(store->GetUniStoreTitle(), entry->GetTitle()));

			/* Clubs. */
			} else if (touching(touch, markBox[3])) {
				meta->SetMarks(store->GetUniStoreTitle(), entry->GetTitle(),
					meta->GetMarks(store->GetUniStoreTitle(), entry->GetTitle()) ^ favoriteMarks::CLUBS);
				entry->SetMark(meta->GetMarks(store->GetUniStoreTitle(), entry->GetTitle()));

			/* Spade. */
			} else if (touching(touch, markBox[4])) {
				meta->SetMarks(store->GetUniStoreTitle(), entry->GetTitle(),
					meta->GetMarks(store->GetUniStoreTitle(), entry->GetTitle()) ^ favoriteMarks::SPADE);

				entry->SetMark(meta->GetMarks(store->GetUniStoreTitle(), entry->GetTitle()));
			}
		}

		if (hDown & KEY_B) showMark = false; // Return back to screen.
	}
}