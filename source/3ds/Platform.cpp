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


#include "Platform.hpp"

#include "gui.hpp"
#include "Utils.hpp"

#include <dirent.h>


/*
	Initialize everything as needed.
*/
void Platform::Initialize(char *ARGV[]) {
	(void)ARGV;

	romfsInit();
	gfxInitDefault();
	Gui::init();
	acInit();
	hidSetRepeatParameters(20, 8);
	osSetSpeedupEnable(true); // Enable speed-up for New 3DS users.
};

void Platform::Exit() {
	acExit();
	Gui::exit();
	gfxExit();
	romfsExit();
}

bool Platform::WiFi::Connected() {
	// return true; // For Citra

	uint32_t WifiStatus;
	return R_SUCCEEDED(ACU_GetWifiStatus(&WifiStatus)) && WifiStatus;
}
