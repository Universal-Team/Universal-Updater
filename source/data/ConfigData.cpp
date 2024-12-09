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

#include "ConfigData.hpp"

#include "Platform.hpp"
#include "UniversalUpdater.hpp"

#include <unistd.h>


/* Detects system language and is used later to set app language to system language. */
std::string ConfigData::SysLang(void) const {
	uint8_t Language = 1;
	#ifdef __3DS__ // 3DS only for now, does set english on NDS.
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
		this->LastStore(this->Get<std::string>("LastStore", this->LastStore()));
		this->List(this->Get<bool>("List", this->List()));
		this->AutoUpdate(this->Get<bool>("AutoUpdate", this->AutoUpdate()));
		this->MetaData(this->Get<bool>("MetaData", this->MetaData()));
	}
};


/* Initializes the Configuration file properly as a JSON. */
void ConfigData::Initialize() {
	FILE *Temp = fopen(CONFIG_PATH, "w");

	const nlohmann::json OBJ = {
		{ "LastStore", this->LastStore() },
		{ "List", this->List() },
		{ "AutoUpdate", this->AutoUpdate() },
		{ "MetaData", this->MetaData() }
	};

	const std::string Dump = OBJ.dump(1, '\t');
	fwrite(Dump.c_str(), 1, Dump.size(), Temp);
	fclose(Temp);
};


/* Save changes to the Configuration, if changes made. */
void ConfigData::Sav() {
	if (this->ChangesMade) {
		FILE *Out = fopen(CONFIG_PATH, "w");

		this->Set<std::string>("LastStore", this->LastStore());
		this->Set<bool>("List", this->List());
		this->Set<bool>("AutoUpdate", this->AutoUpdate());
		this->Set<bool>("MetaData", this->MetaData());

		/* Write changes to file. */
		const std::string Dump = this->CFG.dump(1, '\t');
		fwrite(Dump.c_str(), 1, Dump.size(), Out);
		fclose(Out);
	}
};