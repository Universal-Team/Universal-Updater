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

#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <3ds.h>
#include <memory>

class screen
{
public:
	virtual ~screen() {}
	virtual void Logic(u32 hDown, u32 hHeld, touchPosition touch) = 0;
	virtual void Draw() const = 0;
private:
};

namespace Screen {
	void set(std::unique_ptr<screen> screen2);
	void fade(std::unique_ptr<screen> screen2, bool fadeout = true);
	void back(void);
	void loop(u32 hDown, u32 hHeld, touchPosition touch);
}

#endif