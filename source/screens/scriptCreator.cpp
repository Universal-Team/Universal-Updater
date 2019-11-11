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

#include "keyboard.hpp"

#include "screens/scriptCreator.hpp"

#include "utils/config.hpp"

// The to editing script.
nlohmann::json editScript;

void ScriptCreator::openJson() {
	FILE* file = fopen("sdmc:/3ds/Universal-Updater/Test.json", "r");
	if(file) editScript = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
}

void ScriptCreator::setBool(const std::string &object, const std::string &key, bool v) {
	editScript[object][key] = v;
}

void ScriptCreator::setInt(const std::string &object, const std::string &key, int v) {
	editScript[object][key] = v;
}

void ScriptCreator::setString(const std::string &object, const std::string &key, const std::string &v) {
	editScript[object][key] = v;
}

void ScriptCreator::Draw(void) const {
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("SCRIPTCREATOR"), 400);
	Gui::DrawBottom();
}

// Testing purpose for now.
ScriptCreator::ScriptCreator() {
	openJson();
}

void ScriptCreator::save() {
	FILE* file = fopen("sdmc:/3ds/Universal-Updater/Test.json", "w");
	if(file)	fwrite(editScript.dump(1, '\t').c_str(), 1, editScript.dump(1, '\t').size(), file);
	fclose(file);
}

void ScriptCreator::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_B) {
		save();
		Gui::screenBack();
		return;
	}

	if (hDown & KEY_X) {
		const std::string &test = Input::getString("Enter the Title of the Script.", 50);
		const std::string &test2 = Input::getString("Enter the Author name of the Script.", 50);
		const std::string &test3 = Input::getString("Enter the short description of the Script.", 80);
		const std::string &test4 = Input::getString("Enter the long description of the Script.", 300);

		setString("info", "title", test);
		setString("info", "author", test2);
		setString("info", "shortDesc", test3);
		setString("info", "description", test4);
		setInt("info", "version", 2);
		setInt("info", "revision", 1);
	}
}