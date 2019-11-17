/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 VoltZ, Epicpkmn11, Flame, RocketRobz, TotallyNotGuy
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

#include "gui.hpp"

#include "lang/lang.hpp"

#include "screens/mainMenu.hpp"
#include "screens/screenCommon.hpp"

#include "utils/config.hpp"
#include "utils/sound.h"
#include "utils/structs.hpp"

#include <3ds.h>
#include <dirent.h>
#include <unistd.h>

bool exiting = false;
bool dspFound = false;
touchPosition touch;
sound *bgm = NULL;
bool songIsFound = false;


// If button Position pressed -> Do something.
bool touching(touchPosition touch, Structs::ButtonPos button) {
	if (touch.px >= button.x && touch.px <= (button.x + button.w) && touch.py >= button.y && touch.py <= (button.y + button.h))
		return true;
	else
		return false;
}


void loadSoundEffects(void) {
	if (dspFound == true) {
		if( access( Config::MusicPath.c_str(), F_OK ) != -1 ) {
			bgm = new sound(Config::MusicPath, 1, true);
			songIsFound = true;
		}
	}
}

void playMusic(void) {
	if (songIsFound == true) {
		bgm->play();
	}
}

void stopMusic(void) {
	if (songIsFound == true) {
		bgm->stop();
	}
}

int main()
{
	gfxInitDefault();
	romfsInit();
	Gui::init();
	sdmcInit();
	cfguInit();
	acInit();
	// Create Folder if missing.
	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/Universal-Updater", 0777);
	mkdir("sdmc:/3ds/Universal-Updater/scripts", 0777);

	// We need to make sure, the file exist.
	if(access("sdmc:/3ds/Universal-Updater/Settings.json", F_OK) == -1 ) {
		Config::initializeNewConfig();
	}
	Config::load();
	Lang::load(Config::lang);
	Gui::setScreen(std::make_unique<MainMenu>());
	osSetSpeedupEnable(true);	// Enable speed-up for New 3DS users

 	if( access( "sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) {
		ndspInit();
		dspFound = true;
		loadSoundEffects();
		playMusic();
	 }


	// Loop as long as the status is not exit
	while (aptMainLoop() && !exiting)
	{
		hidScanInput();
		u32 hHeld = hidKeysHeld();
		u32 hDown = hidKeysDown();
		hidTouchRead(&touch);
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, BLACK);
		C2D_TargetClear(bottom, BLACK);
		Gui::clearTextBufs();
		Gui::mainLoop(hDown, hHeld, touch);
		C3D_FrameEnd(0);
		gspWaitForVBlank();
	}

	if (songIsFound == true) {
		stopMusic();
	}
	delete bgm;
	if (dspFound == true) {
		ndspExit();
	}
	Config::save();
	Gui::exit();
	gfxExit();
	cfguExit();
	acExit();
	romfsExit();
	sdmcExit();
	return 0;
}