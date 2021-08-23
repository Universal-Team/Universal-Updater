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

#include "GFXData.hpp"

#include "Common.hpp"
#include "gui.hpp"

#include <nds.h>


GFXData::GFXData() {
	const std::string Images[] = {
		"nitro:/graphics/download.gfx",
		"nitro:/graphics/info.gfx",
		"nitro:/graphics/noIcon.gfx",
		"nitro:/graphics/queue.gfx",
		"nitro:/graphics/search.gfx",
		"nitro:/graphics/settings.gfx",
		"nitro:/graphics/sort.gfx"
	};

	for (const std::string &Img : Images)
		this->Sprites.emplace_back(Image(Img));
};


GFXData::~GFXData() { };


void GFXData::DrawTop() {
	Gui::ScreenDraw(true);
	Gui::Draw_Rect(0, 0, 320, 25, BAR_COLOR);
	Gui::Draw_Rect(0, 26, 320, 214, BG_COLOR);
	Gui::Draw_Rect(0, 25, 320, 1, BAR_OUTLINE);
};


void GFXData::DrawBottom() {
	Gui::ScreenDraw(false);
	Gui::Draw_Rect(0, 0, 320, 240, BG_COLOR);
};


void GFXData::DrawSprite(const int Idx, const int X, const int Y) {
	if (Idx < (int)this->Sprites.size()) this->Sprites[Idx].draw(X, Y, false);
};


void GFXData::DrawSpriteBlend(const int Idx, const int X, const int Y, int color) {
	if (Idx < (int)this->Sprites.size()) {
		if (color) color -= this->Sprites[Idx].palOfs() + 1;
		this->Sprites[Idx].drawSpecial(X, Y, 1.0f, 1.0f, color, false);
	}
};
