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

#include "mainMenu.hpp"
#include "scriptlist.hpp"
#include "startup.hpp"
#include "unistore.hpp"
#include <unistd.h>

extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern int fadealpha;
extern bool fadein;
extern std::unique_ptr<Config> config;

Startup::Startup(int mode, std::string file) {
	this->mode = mode; this->file = file;
}

void Startup::Draw(void) const {
	GFX::DrawTop();
	GFX::DrawSprite(sprites_dev_by_idx, 0, 25);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}

void Startup::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (this->delay > 0) {
		this->delay -= 2;
		if (this->delay <= 0) {
			if (this->mode == 0) {
				config->firstStartup(false);
				Gui::setScreen(std::make_unique<MainMenu>(), true, true);
			} else if (this->mode == 1) {
				config->firstStartup(false);
				if (access(this->file.c_str(), F_OK) == 0) {
					Gui::setScreen(std::make_unique<UniStore>(true, this->file), true, true);
				} else {
					Gui::setScreen(std::make_unique<MainMenu>(), true, true);
				}
			} else if (this->mode == 2) {
				config->firstStartup(false);
				if (access(this->file.c_str(), F_OK) == 0) {
					Gui::setScreen(std::make_unique<ScriptList>(), true, true);
				} else {
					Gui::setScreen(std::make_unique<MainMenu>(), true, true);
				}
			}
		}
	}
}