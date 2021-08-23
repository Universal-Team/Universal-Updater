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

#include "Common.hpp"
#include "ConfigData.hpp"
#include <unistd.h>

#ifdef _3DS
	#define CONFIG_PATH "sdmc:/3ds/Universal-Updater/Config.json"

#elif ARM9
	#define CONFIG_PATH "/_nds/Universal-Updater/Config.json"
#endif


/* Detects system language and is used later to set app language to system language. */
std::string ConfigData::SysLang(void) const {
	uint8_t Language = 1;
	#ifdef _3DS // 3DS only for now, does set english on NDS.
		CFGU_GetSystemLanguage(&Language);
	#endif

	switch(Language) {
		case 0:
			return "ja"; // Japanese.

		case 1:
		default:
			return "en"; // English.

		case 2:
			return "fr"; // French.

		case 3:
			return "de"; // German.

		case 4:
			return "it"; // Italian.

		case 5:
			return "es"; // Spanish.

		case 6:
			return "zh-CN"; // Chinese (Simplified).

		case 7:
			return "ko"; // Korean.

		case 8:
			return "nl"; // Dutch.

		case 9:
			return "pt"; // Portuguese.

		case 10:
			return "ru"; // Russian.

		case 11:
			return "zh-TW"; // Chinese (Traditional).
	}
};


/* Loads the Configuration file. */
void ConfigData::Load() {
	if (access(CONFIG_PATH, F_OK) != 0) this->Initialize();

	FILE *File = fopen(CONFIG_PATH, "rt");
	this->CFG = nlohmann::json::parse(File, nullptr, false);
	fclose(File);

	if (!this->CFG.is_discarded()) {
		/* TODO: Init things. */
	}
};


/* Initializes the Configuration file properly as a JSON. */
void ConfigData::Initialize() {
	FILE *Temp = fopen(CONFIG_PATH, "w");

	const nlohmann::json OBJ = {
		/* TODO: Fill with initialize stuff. */
	};

	const std::string Dump = OBJ.dump(1, '\t');
	fwrite(Dump.c_str(), 1, Dump.size(), Temp);
	fclose(Temp);
};


/* Save changes to the Configuration, if changes made. */
void ConfigData::Sav() {
	if (this->ChangesMade) {
		FILE *Out = fopen(CONFIG_PATH, "w");

		/* Write changes to file. */
		const std::string Dump = this->CFG.dump(1, '\t');
		fwrite(Dump.c_str(), 1, Dump.size(), Out);
		fclose(Out);
	}
};