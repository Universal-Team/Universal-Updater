/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "screens/screen.hpp"

#include <stack>

// Fade stuff.
int fadealpha = 255;
bool fadein = true;

std::stack<std::unique_ptr<screen>> screens;

// Set a specific Screen.
void Screen::set(std::unique_ptr<screen> screen2)
{
	screens.push(std::move(screen2));
}

// Fade into another Screen, but first do a fadeout.
void Screen::fade(std::unique_ptr<screen> screen2, bool fadeout) {
	if (fadeout) {
		fadealpha += 6;
		if (fadealpha > 255) {
			fadealpha = 255;
			screens.push(std::move(screen2));
			fadein = true;
			fadeout = false;
		}
	}
}

// Go a Screen back.
void Screen::back()
{
	screens.pop();
}

// For the Mainloop.
void Screen::loop(u32 hDown, u32 hHeld, touchPosition touch) {
	screens.top()->Draw();
	screens.top()->Logic(hDown, hHeld, touch);
	if (fadein == true) {
		fadealpha -= 6;
		if (fadealpha < 0) {
			fadealpha = 0;
			fadein = false;
		}
	}
}