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

#include "colorHelper.hpp"
#include "common.hpp"
#include "config.hpp"
#include <citro2d.h>
#include <unistd.h>

// Used to add missing stuff for the JSON.
void Config::addMissingThings() {
	if (this->json["VERSION"] < 2) {
		this->setString("3DSX_PATH", _3DSX_PATH);
		this->setString("NDS_PATH", _NDS_PATH);
		this->setString("ARCHIVE_PATH", ARCHIVES_DEFAULT);
		this->setBool("CITRA", false);
	}
}

//Detects system language and is used later to set app language to system language
void Config::sysLang() {
  u8 language = 0;

  CFGU_GetSystemLanguage(&language);
  switch(language) {
    case 0:
      this->language("jp");
      break;
    case 1:
      this->language("en");
      break;
    case 2:
      this->language("fr");
      break;
    case 3:
      this->language("de");
      break;
    case 4:
      this->language("it");
      break;
	case 5:
      this->language("es");
      break;
	case 6:
      this->language("en"); //Simplified chinese, not translated
      break;
	case 7:
      this->language("en"); //Korean, not translated
      break;
	case 8:
      this->language("nl");
      break;
	case 9:
      this->language("pt");
      break;
	case 10:
      this->language("ru");
      break;
	case 11:
      this->language("en"); //traditional chinese, not translated
      break;
  }
}


// In case it doesn't exist.
void Config::initialize() {
	// Create through fopen "Write".
	FILE *file = fopen("sdmc:/3ds/Universal-Updater/Settings.json", "w");

	// Set default values.
	this->setInt("BARCOLOR", BarColor);
	this->setInt("TOPBGCOLOR", TopBGColor);
	this->setInt("BOTTOMBGCOLOR", BottomBGColor);
	this->setInt("TEXTCOLOR", WHITE);
	this->setInt("BUTTON", C2D_Color32(0, 0, 50, 255));
	this->setInt("SELECTEDCOLOR", SelectedColordefault);
	this->setInt("UNSELECTEDCOLOR", UnselectedColordefault);
	this->setString("SCRIPTPATH", SCRIPTS_PATH);
	this->setInt("LANGPATH", 0);
	this->setInt("VIEWMODE", 0);
	this->setInt("PROGRESSBARCOLOR", WHITE);
	this->setString("MUSICPATH", MUSIC_PATH);
	this->setBool("LOGGING", false);
	this->setBool("BARS", true);
	this->setInt("AUTOBOOT", 0);
	this->setString("STOREPATH", STORE_PATH);
	this->setString("AUTOBOOT_FILE", "");
	this->setInt("OUTDATED", C2D_Color32(0xfb, 0x5b, 0x5b, 255));
	this->setInt("UPTODATE", C2D_Color32(0xa5, 0xdd, 0x81, 255));
	this->setInt("NOTFOUND", C2D_Color32(255, 128, 0, 255));
	this->setInt("FUTURE", C2D_Color32(255, 255, 0, 255));
	this->setInt("KEY_DELAY", 5);
	this->setBool("SCREEN_FADE", false);
	this->setBool("PROGRESS_DISPLAY", true);
	this->sysLang();
	this->setBool("FIRST_STARTUP", true);
	this->setBool("USE_SCRIPT_COLORS", true);
	this->setBool("SHOW_SPEED", false);
	this->setString("3DSX_PATH", _3DSX_PATH);
	this->setString("NDS_PATH", _NDS_PATH);
	this->setString("ARCHIVE_PATH", ARCHIVES_DEFAULT);
	this->setBool("CITRA", false);
	this->setInt("VERSION", this->configVersion);

	// Write to file.
	const std::string dump = this->json.dump(1, '\t');
	fwrite(dump.c_str(), 1, this->json.dump(1, '\t').size(), file);
	fclose(file); // Now we have the file and can properly access it.
}

Config::Config() {
	if (access("sdmc:/3ds/Universal-Updater/Settings.json", F_OK) != 0 ) {
		this->initialize();
	}

	FILE* file = fopen("sdmc:/3ds/Universal-Updater/Settings.json", "r");
	this->json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	if (!this->json.contains("VERSION")) {
		// Let us create a new one.
		this->initialize();
	}

	// Here we add the missing things.
	if (this->json["VERSION"] < this->configVersion) {
		this->addMissingThings();
	}

	if (!this->json.contains("BARCOLOR")) {
		this->barColor(BarColor);
	} else {
		this->barColor(this->getInt("BARCOLOR"));
	}

	if (!this->json.contains("TOPBGCOLOR")) {
		this->topBG(TopBGColor);
	} else {
		this->topBG(this->getInt("TOPBGCOLOR"));
	}

	if (!this->json.contains("BOTTOMBGCOLOR")) {
		this->bottomBG(BottomBGColor);
	} else {
		this->bottomBG(this->getInt("BOTTOMBGCOLOR"));
	}

	if (!this->json.contains("TEXTCOLOR")) {
		this->textColor(WHITE);
	} else {
		this->textColor(this->getInt("TEXTCOLOR"));
	}

	if (!this->json.contains("BUTTON")) {
		this->buttonColor(C2D_Color32(0, 0, 50, 255));
	} else {
		this->buttonColor(this->getInt("BUTTON"));
	}

	if (!this->json.contains("SELECTEDCOLOR")) {
		this->selectedColor(SelectedColordefault);
	} else {
		this->selectedColor(this->getInt("SELECTEDCOLOR"));
	}

	if (!this->json.contains("UNSELECTEDCOLOR")) {
		this->unselectedColor(UnselectedColordefault);
	} else {
		this->unselectedColor(this->getInt("UNSELECTEDCOLOR"));
	}

	if (!this->json.contains("SCRIPTPATH")) {
		this->scriptPath(SCRIPTS_PATH);
	} else {
		this->scriptPath(this->getString("SCRIPTPATH"));
	}

	if (!this->json.contains("LANGPATH")) {
		this->langPath(0);
	} else {
		this->langPath(this->getInt("LANGPATH"));
	}

	if (!this->json.contains("VIEWMODE")) {
		this->viewMode(0);
	} else {
		this->viewMode(this->getInt("VIEWMODE"));
	}

	if (!this->json.contains("PROGRESSBARCOLOR")) {
		this->progressbarColor(WHITE);
	} else {
		this->progressbarColor(this->getInt("PROGRESSBARCOLOR"));
	}

	if (!this->json.contains("MUSICPATH")) {
		this->musicPath(MUSIC_PATH);
	} else {
		this->musicPath(this->getString("MUSICPATH"));
	}

	if (!this->json.contains("LOGGING")) {
		this->logging(false);
	} else {
		this->logging(this->getBool("LOGGING"));
	}

	if (!this->json.contains("BARS")) {
		this->useBars(true);
	} else {
		this->useBars(this->getBool("BARS"));
	}

	if (!this->json.contains("AUTOBOOT")) {
		this->autoboot(0);
	} else {
		this->autoboot(this->getInt("AUTOBOOT"));
	}

	if (!this->json.contains("STOREPATH")) {
		this->storePath(STORE_PATH);
	} else {
		this->storePath(this->getString("STOREPATH"));
	}

	if (!this->json.contains("AUTOBOOT_FILE")) {
		this->autobootFile("");
	} else {
		this->autobootFile(this->getString("AUTOBOOT_FILE"));
	}

	if (!this->json.contains("OUTDATED")) {
		this->outdatedColor(C2D_Color32(0xfb, 0x5b, 0x5b, 255));
	} else {
		this->outdatedColor(this->getInt("OUTDATED"));
	}

	if (!this->json.contains("UPTODATE")) {
		this->uptodateColor(C2D_Color32(0xa5, 0xdd, 0x81, 255));
	} else {
		this->uptodateColor(this->getInt("UPTODATE"));
	}

	if (!this->json.contains("NOTFOUND")) {
		this->notfoundColor(C2D_Color32(255, 128, 0, 255));
	} else {
		this->notfoundColor(this->getInt("NOTFOUND"));
	}

	if (!this->json.contains("FUTURE")) {
		this->futureColor(C2D_Color32(255, 255, 0, 255));
	} else {
		this->futureColor(this->getInt("FUTURE"));
	}

	if (!this->json.contains("KEY_DELAY")) {
		this->keyDelay(5);
	} else {
		this->keyDelay(this->getInt("KEY_DELAY"));
	}

	if (!this->json.contains("SCREEN_FADE")) {
		this->screenFade(false);
	} else {
		this->screenFade(this->getBool("SCREEN_FADE"));
	}

	if (!this->json.contains("PROGRESS_DISPLAY")) {
		this->progressDisplay(true);
	} else {
		this->progressDisplay(this->getBool("PROGRESS_DISPLAY"));
	}

	if (!this->json.contains("LANGUAGE") || this->json.at("LANGUAGE").is_number()) {
		this->sysLang();
		this->initialChanges = true;
	} else {
		this->language(this->getString("LANGUAGE"));
	}

	if (!this->json.contains("FIRST_STARTUP")) {
		this->firstStartup(true);
	} else {
		this->firstStartup(this->getBool("FIRST_STARTUP"));
	}

	if (!this->json.contains("USE_SCRIPT_COLORS")) {
		this->useScriptColor(true);
	} else {
		this->useScriptColor(this->getBool("USE_SCRIPT_COLORS"));
	}

	if (!this->json.contains("SHOW_SPEED")) {
		this->showSpeed(false);
	} else {
		this->showSpeed(this->getBool("SHOW_SPEED"));
	}

	if (!this->json.contains("3DSX_PATH")) {
		this->_3dsxpath(_3DSX_PATH);
	} else {
		this->_3dsxpath(this->getString("3DSX_PATH"));
	}

	if (!this->json.contains("NDS_PATH")) {
		this->ndspath(_NDS_PATH);
	} else {
		this->ndspath(this->getString("NDS_PATH"));
	}

	if (!this->json.contains("ARCHIVE_PATH")) {
		this->archivepath(ARCHIVES_DEFAULT);
	} else {
		this->archivepath(this->getString("ARCHIVE_PATH"));
	}

	if (!this->json.contains("CITRA")) {
		this->citra(false);
	} else {
		this->citra(this->getBool("CITRA"));
	}

	this->changesMade = false; // No changes made yet.
}

// Write to config if changesMade.
void Config::save() {
	if (this->changesMade) {
		this->changesMade = false;
		FILE *file = fopen("sdmc:/3ds/Universal-Updater/Settings.json", "w");
		// Set values.
		this->setInt("BARCOLOR", this->barColor());
		this->setInt("TOPBGCOLOR", this->topBG());
		this->setInt("BOTTOMBGCOLOR", this->bottomBG());
		this->setInt("TEXTCOLOR", this->textColor());
		this->setInt("BUTTON", this->buttonColor());
		this->setInt("SELECTEDCOLOR", this->selectedColor());
		this->setInt("UNSELECTEDCOLOR", this->unselectedColor());
		this->setString("SCRIPTPATH", this->scriptPath());
		this->setInt("LANGPATH", this->langPath());
		this->setInt("VIEWMODE", this->viewMode());
		this->setInt("PROGRESSBARCOLOR", this->progressbarColor());
		this->setString("MUSICPATH", this->musicPath());
		this->setBool("LOGGING", this->logging());
		this->setBool("BARS", this->useBars());
		this->setInt("AUTOBOOT", this->autoboot());
		this->setString("STOREPATH", this->storePath());
		this->setString("AUTOBOOT_FILE", this->autobootFile());
		this->setInt("OUTDATED", this->outdatedColor());
		this->setInt("UPTODATE", this->uptodateColor());
		this->setInt("NOTFOUND", this->notfoundColor());
		this->setInt("FUTURE", this->futureColor());
		this->setInt("KEY_DELAY", this->keyDelay());
		this->setBool("SCREEN_FADE", this->screenFade());
		this->setBool("PROGRESS_DISPLAY", this->progressDisplay());
		this->setString("LANGUAGE", this->language());
		this->setBool("FIRST_STARTUP", this->firstStartup());
		this->setBool("USE_SCRIPT_COLORS", this->useScriptColor());
		this->setBool("SHOW_SPEED", this->showSpeed());
		this->setString("3DSX_PATH", this->_3dsxpath());
		this->setString("NDS_PATH", this->ndspath());
		this->setString("ARCHIVE_PATH", this->archivepath());
		this->setBool("CITRA", this->citra());
		// Write changes to file.
		const std::string dump = this->json.dump(1, '\t');
		fwrite(dump.c_str(), 1, this->json.dump(1, '\t').size(), file);
		fclose(file);
	}
}

// Helper functions.
bool Config::getBool(const std::string &key) {
	if (!this->json.contains(key)) {
		return false;
	}

	return this->json.at(key).get_ref<const bool&>();
}
void Config::setBool(const std::string &key, bool v) {
	this->json[key] = v;
}

int Config::getInt(const std::string &key) {
	if (!this->json.contains(key)) {
		return 0;
	}

	return this->json.at(key).get_ref<const int64_t&>();
}
void Config::setInt(const std::string &key, int v) {
	this->json[key] = v;
}

std::string Config::getString(const std::string &key) {
	if (!this->json.contains(key)) {
		return "";
	}
	
	return this->json.at(key).get_ref<const std::string&>();
}
void Config::setString(const std::string &key, const std::string &v) {
	this->json[key] = v;
}
