/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "config.hpp"
#include "init.hpp"
#include "gfx.hpp"
#include "gui.hpp"
#include "lang.hpp"
#include "logging.hpp"
#include "mainMenu.hpp"
#include "screenCommon.hpp"
#include "sound.h"

#include <3ds.h>
#include <dirent.h>
#include <unistd.h>

// The classic Fade Effect! ;P
int fadealpha = 255;
bool fadein = true;

bool exiting = false;
bool dspFound = false;
touchPosition touch;
sound *bgm = NULL;
bool songIsFound = false;

// Include all spritesheet's.
C2D_SpriteSheet sprites;

// If button Position pressed -> Do something.
bool touching(touchPosition touch, Structs::ButtonPos button) {
	if (touch.px >= button.x && touch.px <= (button.x + button.w) && touch.py >= button.y && touch.py <= (button.y + button.h))
		return true;
	else
		return false;
}

void Init::loadSoundEffects(void) {
	if (dspFound == true) {
		if( access( Config::MusicPath.c_str(), F_OK ) != -1 ) {
			bgm = new sound(Config::MusicPath, 1, true);
			songIsFound = true;
		}
	}
}

void Init::playMusic(void) {
	if (songIsFound == true) {
		bgm->play();
	}
}

void Init::stopMusic(void) {
	if (songIsFound == true) {
		bgm->stop();
	}
}


Result Init::Initialize() {
	gfxInitDefault();
	romfsInit();
	Gui::init();
	Gui::loadSheet("romfs:/gfx/sprites.t3x", sprites);
	sdmcInit();
	cfguInit();
	acInit();
	// Create Folder if missing.
	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/Universal-Updater", 0777);
	mkdir("sdmc:/3ds/Universal-Updater/scripts", 0777);
	mkdir("sdmc:/3ds/Universal-Updater/stores", 0777);

	// We need to make sure, the file exist.
	if(access("sdmc:/3ds/Universal-Updater/Settings.json", F_OK) == -1 ) {
		Config::initializeNewConfig();
	}
	Config::load();
	Lang::load(Config::lang);

	if (Config::Logging == true) {
		Logging::createLogFile();
	}

	Gui::setScreen(std::make_unique<MainMenu>());
	osSetSpeedupEnable(true);	// Enable speed-up for New 3DS users

 	if( access( "sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) {
		ndspInit();
		dspFound = true;
		loadSoundEffects();
		playMusic();
	}

    return 0;
}

Result Init::MainLoop() {
    // Initialize everything.
    Initialize();

	// Loop as long as the status is not exiting.
	while (aptMainLoop() && !exiting)
	{
		hidScanInput();
		u32 hHeld = hidKeysHeld();
		u32 hDown = hidKeysDown();
		hidTouchRead(&touch);
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, BLACK);
		C2D_TargetClear(Bottom, BLACK);
		Gui::clearTextBufs();
		Gui::mainLoop(hDown, hHeld, touch);
		C3D_FrameEnd(0);
		gspWaitForVBlank();

		if (fadein == true) {
			fadealpha -= 3;
			if (fadealpha < 0) {
				fadealpha = 0;
				fadein = false;
			}
		}
	}
    // Exit all services and exit the app.
    Exit();
    return 0;
}

Result Init::Exit() {
	if (songIsFound == true) {
		stopMusic();
	}
	delete bgm;
	if (dspFound == true) {
		ndspExit();
	}
	Config::save();
	Gui::exit();
	Gui::unloadSheet(sprites);
	gfxExit();
	cfguExit();
	acExit();
	romfsExit();
	sdmcExit();
	return 0;
}