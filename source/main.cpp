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

#include "argumentParser.hpp"
#include "common.hpp"
#include "init.hpp"
#include <dirent.h>
#include <string>

#define ARG_AMOUNT 4 // In case for more args, change this. It must be ARG amount + 1, because of 3DSX Path.
std::string _3dsxPath = "";

/* ARG Init. */
static void InitForARG() {
	gfxInitDefault();
	romfsInit();
	cfguInit();
	Gui::init();
	amInit();
	acInit();

	/* Create Directories, if missing. */
	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/Universal-Updater", 0777);
	mkdir("sdmc:/3ds/Universal-Updater/stores", 0777);
	mkdir("sdmc:/3ds/Universal-Updater/shortcuts", 0777);

	config = std::make_unique<Config>();
	GFX::SelectedTheme = config->theme();
	if (GFX::SelectedTheme > (_THEME_AMOUNT - 1)) GFX::SelectedTheme = 0; // In case it is above the max themes.
	Lang::load(config->language());
	Init::LoadFont();
	osSetSpeedupEnable(true); // Enable speed-up for New 3DS users.
}

/* ARG Exit. */
static Result ExitForARG() {
	Gui::exit();
	Init::UnloadFont();
	gfxExit();
	cfguExit();
	acExit();
	amExit();
	romfsExit();

	return 0;
}

int main(int argc, char *argv[]) {
	if (argc > 0) _3dsxPath = argv[0];

	/* 4 --> Argument mode. */
	if (argc == ARG_AMOUNT) {
		InitForARG();

		const std::string file = argv[1];
		const std::string entry = argv[2];
		int dlIndex = atoi(argv[3]);

		std::unique_ptr<ArgumentParser> arg = std::make_unique<ArgumentParser>(file, entry, dlIndex);

		if (arg->GetValid()) arg->Execute(); // Execute, if valid.
		else Msg::waitMsg(Lang::get("ARGUMENT_INVALID"));
		return ExitForARG();
	}

	return Init::MainLoop();
}