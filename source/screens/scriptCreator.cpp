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

#include "utils/fileBrowse.hpp"
#include "keyboard.hpp"
#include "logging.hpp"
#include "scriptCreator.hpp"

#include <fstream>
#include <unistd.h>

// The to editing script.
nlohmann::json editScript;
std::string entryName = ""; // So we can set to *that* entry.

void ScriptCreator::openJson(std::string fileName) {
	std::string scriptFile = Config::ScriptPath + fileName;
	FILE* file = fopen(scriptFile.c_str(), "r");
	if(file) editScript = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
}

// BOOL.
void ScriptCreator::setBool(const std::string &object, const std::string &key, bool v) {
	editScript[object][key] = v;
}
void ScriptCreator::setBool2(const std::string &object, const std::string &key, const std::string &key2, bool v) {
	editScript[object][key][key2] = v;
}


// INT.
void ScriptCreator::setInt(const std::string &object, const std::string &key, int v) {
	editScript[object][key] = v;
}
void ScriptCreator::setInt2(const std::string &object, const std::string &key, const std::string &key2, int v) {
	editScript[object][key][key2] = v;
}

// STRING
void ScriptCreator::setString(const std::string &object, const std::string &key, const std::string &v) {
	editScript[object][key] = v;
}
void ScriptCreator::setString2(const std::string &object, const std::string &key, const std::string &key2, const std::string &v) {
	editScript[object][key][key2] = v;
}

void ScriptCreator::Draw(void) const {
	if (mode == 0) {
		DrawSubMenu();
	} else if (mode == 1) {
		DrawScriptScreen();
	}
}

void ScriptCreator::DrawSubMenu(void) const {
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, Lang::get("SCRIPTCREATOR"), 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("SCRIPTCREATOR"), 400);
	}

	GFX::DrawBottom();

	for (int i = 0; i < 2; i++) {
		if (Selection == i) {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, Config::SelectedColor);
		} else {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, Config::UnselectedColor);
		}
	}

	Gui::DrawString((320-Gui::GetStringWidth(0.6f, "New script"))/2, mainButtons[0].y+10, 0.6f, Config::TxtColor, "New script", 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, "Existing script"))/2, mainButtons[1].y+10, 0.6f, Config::TxtColor, "Existing script", 140);
}

void ScriptCreator::DrawScriptScreen(void) const {
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, "Selected Entry: " + entryName, 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Selected Entry: " + entryName, 400);
	}
	
	GFX::DrawBottom();

	// Draw Page.
	for (int i = 0; i < 2; i++) {
		if (i == page) {
			Gui::DrawString(260, 3, 0.6f, Config::TxtColor, std::to_string(i+1) + " / 2", 140);
		}
	}

	if (page == 0) {
		for (int i = 0; i < 6; i++) {
			if (Selection == i) {
				Gui::Draw_Rect(creatorButtons[i].x, creatorButtons[i].y, creatorButtons[i].w, creatorButtons[i].h, Config::SelectedColor);
			} else {
				Gui::Draw_Rect(creatorButtons[i].x, creatorButtons[i].y, creatorButtons[i].w, creatorButtons[i].h, Config::UnselectedColor);
			}
		}

		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "downloadRelease"))/2-150+70, creatorButtons[0].y+10, 0.6f, Config::TxtColor, "downloadRelease", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "downloadFile"))/2+150-70, creatorButtons[1].y+10, 0.6f, Config::TxtColor, "downloadFile", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "deleteFile"))/2-150+70, creatorButtons[2].y+10, 0.6f, Config::TxtColor, "deleteFile", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "extractFile"))/2+150-70, creatorButtons[3].y+10, 0.6f, Config::TxtColor, "extractFile", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "installCia"))/2-150+70, creatorButtons[4].y+10, 0.6f, Config::TxtColor, "installCia", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "mkdir"))/2+150-70, creatorButtons[5].y+10, 0.6f, Config::TxtColor, "mkdir", 140);
	} else if (page == 1) {
		for (int i = 0; i < 3; i++) {
			if (Selection == i) {
				Gui::Draw_Rect(creatorButtons[i].x, creatorButtons[i].y, creatorButtons[i].w, creatorButtons[i].h, Config::SelectedColor);
			} else {
				Gui::Draw_Rect(creatorButtons[i].x, creatorButtons[i].y, creatorButtons[i].w, creatorButtons[i].h, Config::UnselectedColor);
			}
		}

		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "rmdir"))/2-150+70, creatorButtons[0].y+10, 0.6f, Config::TxtColor, "rmdir", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "mkfile"))/2+150-70, creatorButtons[1].y+10, 0.6f, Config::TxtColor, "mkfile", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "TimeMsg"))/2-150+70, creatorButtons[2].y+10, 0.6f, Config::TxtColor, "TimeMsg", 140);
	}
}

void ScriptCreator::createNewJson(std::string fileName) {
	std::ofstream ofstream;
	ofstream.open(fileName.c_str(), std::ofstream::out | std::ofstream::app);
	ofstream.close();
}

// Test.
void ScriptCreator::createDownloadRelease() {
	// Repo.
	std::string repo = Input::getString(50, "Enter the name of the Owner.");
	repo += "/";
	repo += Input::getString(50, "Enter the name of the repo.");
	// File.
	std::string file = Input::getString(50, "Enter the name of the file.");
	// Output.
	std::string output = Input::getString(50, "Enter the name of the Output path.");
	// Prerelease.
	bool prerelease = true;
	// Message.
	std::string message = Input::getString(50, "Enter the Message.");

	editScript[entryName] = { {{"type", "downloadRelease"}, {"repo", repo}, {"file", file}, {"output", output}, {"includePrerelease", prerelease}, {"message", message}} };
	Logging::writeToLog("Execute 'ScriptCreator::createDownloadRelease();'.");
}

// To-Do.

void ScriptCreator::createDownloadFile() {
	// URL of the file.
	std::string file = Input::getString(50, "Enter the URL of the file.");
	// Output.
	std::string output = Input::getString(50, "Enter the name of the Output path.");
	// Message.
	std::string message = Input::getString(50, "Enter the Message.");

	editScript[entryName] = { {{"type", "downloadFile"}, {"file", file}, {"output", output}, {"message", message}} };
	Logging::writeToLog("Execute 'ScriptCreator::createDownloadFile();'.");
}


void ScriptCreator::createDeleteFile() {
	// URL of the file.
	std::string file = Input::getString(50, "Enter the path to the file.");
	// Message.
	std::string message = Input::getString(50, "Enter the Message.");

	editScript[entryName] = { {{"type", "deleteFile"}, {"file", file}, {"message", message}} };
	Logging::writeToLog("Execute 'ScriptCreator::createDeleteFile();'.");
}


void ScriptCreator::createExtractFile() {
	// File path.
	std::string file = Input::getString(50, "Enter the path to the file.");
	// Input of the archive.
	std::string input = Input::getString(50, "Enter the Input of what should be extracted.");
	// Output path.
	std::string output = Input::getString(50, "Enter the output path.");
	// Message.
	std::string message = Input::getString(50, "Enter the Message.");

	editScript[entryName] = { {{"type", "extractFile"}, {"file", file}, {"input", input}, {"output", output}, {"message", message}} };
	Logging::writeToLog("Execute 'ScriptCreator::createExtractFile();'.");
}


void ScriptCreator::createInstallCia() {
	// File path.
	std::string file = Input::getString(50, "Enter the path to the CIA File.");
	// Message.
	std::string message = Input::getString(50, "Enter the Message.");

	editScript[entryName] = { {{"type", "installCia"}, {"file", file}, {"message", message}} };
	Logging::writeToLog("Execute 'ScriptCreator::createInstallCia();'.");
}


void ScriptCreator::createMkDir() {
	// Directory path.
	std::string directory = Input::getString(50, "Enter the directory path.");

	editScript[entryName] = { {{"type", "mkdir"}, {"directory", directory}} };
	Logging::writeToLog("Execute 'ScriptCreator::createMkDir();'.");
}

void ScriptCreator::createRmDir() {
	// Directory path.
	std::string directory = Input::getString(50, "Enter the directory path.");

	editScript[entryName] = { {{"type", "rmdir"}, {"directory", directory}} };
	Logging::writeToLog("Execute 'ScriptCreator::createRmDir();'.");
}

void ScriptCreator::createMkFile() {
	// File path.
	std::string file = Input::getString(50, "Enter the path to the new File.");

	editScript[entryName] = { {{"type", "mkfile"}, {"file", file}} };
	Logging::writeToLog("Execute 'ScriptCreator::createMkFile();'.");
}

void ScriptCreator::createTimeMsg() {
	// Message.
	std::string message = Input::getString(50, "Enter the Message.");
	// Seconds.
	int seconds = Input::getUint(999, "Enter the Seconds for the Message to display.");

	editScript[entryName] = { {{"type", "timeMsg"}, {"message", message}, {"seconds", seconds}} };
	Logging::writeToLog("Execute 'ScriptCreator::createTimeMsg();'.");
}


void ScriptCreator::save() {
	FILE* file = fopen(jsonFileName.c_str(), "w");
	if(file)	fwrite(editScript.dump(1, '\t').c_str(), 1, editScript.dump(1, '\t').size(), file);
	fclose(file);
	Logging::writeToLog("Execute 'ScriptCreator::save();'.");
}

// Important to make Scripts valid.
void ScriptCreator::setInfoStuff(void) {
	// Get needed things.
	const std::string &test = Input::getString(50, "Enter the Title of the script.");
	const std::string &test2 = Input::getString(50, "Enter the Author name of the script.");
	const std::string &test3 = Input::getString(80, "Enter the short description of the script.");
	const std::string &test4 = Input::getString(300, "Enter the long description of the script.");
	int scriptRevision = Input::getUint(99, "Enter the script revision.");
	// Set the real JSON stuff.
	setString("info", "title", test);
	setString("info", "author", test2);
	setString("info", "shortDesc", test3);
	setString("info", "description", test4);
	setInt("info", "version", 2);
	setInt("info", "revision", scriptRevision);
}


void ScriptCreator::SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_B) {
		Gui::screenBack();
		return;
	}

	if (hDown & KEY_A) {
		switch(Selection) {
			case 0:
				jsonFileName = Config::ScriptPath;
				jsonFileName += Input::getString(20, "Enter the name of the JSON file.");
				if (jsonFileName != "") {
					jsonFileName += ".json";
					createNewJson(jsonFileName);
					openJson(jsonFileName);
					entryName = Input::getString(50, "Enter the EntryName.");
					Selection = 0;
					mode = 1;
				}
				break;
			case 1:
				std::string tempScript = selectFilePath("Select the Script file.", Config::ScriptPath, {"json"}, 2);
				if (tempScript != "") {
					jsonFileName = tempScript;
					if(access(jsonFileName.c_str(), F_OK) != -1 ) {
						openJson(jsonFileName);
						entryName = Input::getString(50, "Enter the EntryName.");
						Selection = 0;
						mode = 1;
					}
				}
				break;
		}
	}


	if (hDown & KEY_UP) {
		if(Selection == 1)	Selection = 0;
	}

	if (hDown & KEY_DOWN) {
		if(Selection == 0)	Selection = 1;
	}
}

void ScriptCreator::scriptLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_B) {
		save();
		Selection = 0;
		mode = 0;
	}

	// Page 1.
	if (page == 0) {
		if (hDown & KEY_UP) {
			if(Selection > 1)	Selection -= 2;
		}
		if (hDown & KEY_DOWN) {
			if(Selection < 4)	Selection += 2;
		}
		if (hDown & KEY_LEFT) {
			if (Selection%2) Selection--;
		}
		if (hDown & KEY_RIGHT) {
			if (!(Selection%2)) Selection++;
		}
	} else if (page == 1) {
		if (hDown & KEY_UP) {
			if (Selection == 2)	Selection = 0;
		}
		if (hDown & KEY_RIGHT) {
			if (Selection == 0)	Selection = 1;
		}
		if (hDown & KEY_LEFT) {
			if (Selection == 1)	Selection = 0;
		}
		if (hDown & KEY_DOWN) {
			if (Selection == 0)	Selection = 2;
		}
	}

	// Page 2.

	if (hDown & KEY_R) {
		if (page == 0) {
			page = 1;
			Selection = 0;
		}
	}

	if (hDown & KEY_L) {
		if (page == 1) {
			page = 0;
			Selection = 0;
		}
	}

	if (hDown & KEY_A) {
		if (page == 0) {
			switch(Selection) {
				case 0:
					createDownloadRelease();
					break;
				case 1:
					createDownloadFile();
					break;
				case 2:
					createDeleteFile();
					break;
				case 3:
					createExtractFile();
					break;
				case 4:
					createInstallCia();
					break;
				case 5:
					createMkDir();
					break;
			}
		} else if (page == 1) {
			switch(Selection) {
				case 0:
					createRmDir();
					break;
				case 1:
					createMkFile();
					break;
				case 2:
					createTimeMsg();
					break;
			}
		}
	}

	if (hDown & KEY_X) {
		setInfoStuff();
	}
}

void ScriptCreator::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (mode == 0) {
		SubMenuLogic(hDown, hHeld, touch);
	} else if (mode == 1) {
		scriptLogic(hDown, hHeld, touch);
	}
}