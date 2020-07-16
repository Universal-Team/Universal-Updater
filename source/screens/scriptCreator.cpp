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

#include "fileBrowse.hpp"
#include "keyboard.hpp"
#include "logging.hpp"
#include "scriptCreator.hpp"

#include <fstream>
#include <unistd.h>

extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern std::unique_ptr<Config> config;

void ScriptCreator::openJson(std::string fileName) {
	std::string scriptFile = fileName;
	FILE* file = fopen(scriptFile.c_str(), "r");
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek (file, 0, SEEK_SET);
	char data[size + 1];
	fread(data, 1, size, file);
	data[size] = '\0';
	fclose(file);
	this->editScript = nlohmann::json::parse((char*)data, nullptr, false);
}

// BOOL.
void ScriptCreator::setBool(const std::string &object, const std::string &key, bool v) {
	this->editScript[object][key] = v;
}
void ScriptCreator::setBool2(const std::string &object, const std::string &key, const std::string &key2, bool v) {
	this->editScript[object][key][key2] = v;
}


// INT.
void ScriptCreator::setInt(const std::string &object, const std::string &key, int v) {
	this->editScript[object][key] = v;
}
void ScriptCreator::setInt2(const std::string &object, const std::string &key, const std::string &key2, int v) {
	this->editScript[object][key][key2] = v;
}

// STRING
void ScriptCreator::setString(const std::string &object, const std::string &key, const std::string &v) {
	this->editScript[object][key] = v;
}
void ScriptCreator::setString2(const std::string &object, const std::string &key, const std::string &key2, const std::string &v) {
	this->editScript[object][key][key2] = v;
}

void ScriptCreator::Draw(void) const {
	if (this->mode == 0) {
		this->DrawSubMenu();
	} else if (this->mode == 1) {
		this->DrawScriptScreen();
	}
}

void ScriptCreator::DrawSubMenu(void) const {
	GFX::DrawTop();
	Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.7f, config->textColor(), Lang::get("SCRIPTCREATOR"), 400);

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();

	for (int i = 0; i < 2; i++) {
		if (this->Selection == i) {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, config->selectedColor());
		} else {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, config->unselectedColor());
		}
	}

	Gui::DrawString((320-Gui::GetStringWidth(0.6f, "New script"))/2, mainButtons[0].y+10, 0.6f, config->textColor(), "New script", 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, "Existing script"))/2, mainButtons[1].y+10, 0.6f, config->textColor(), "Existing script", 140);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}

void ScriptCreator::DrawScriptScreen(void) const {
	GFX::DrawTop();
	Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.7f, config->textColor(), "Selected Entry: " + entryName, 400);

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();

	// Draw Page.
	for (int i = 0; i < 3; i++) {
		if (i == this->page) {
			Gui::DrawStringCentered(0, 3, 0.6f, config->textColor(), std::to_string(i+1) + " | 3", 140);
		}
	}

	if (this->page == 0) {
		for (int i = 0; i < 6; i++) {
			if (this->Selection == i) {
				Gui::Draw_Rect(creatorButtons[i].x, creatorButtons[i].y, creatorButtons[i].w, creatorButtons[i].h, config->selectedColor());
			} else {
				Gui::Draw_Rect(creatorButtons[i].x, creatorButtons[i].y, creatorButtons[i].w, creatorButtons[i].h, config->unselectedColor());
			}
		}

		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "downloadRelease"))/2-150+70, creatorButtons[0].y+10, 0.6f, config->textColor(), "downloadRelease", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "downloadFile"))/2+150-70, creatorButtons[1].y+10, 0.6f, config->textColor(), "downloadFile", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "deleteFile"))/2-150+70, creatorButtons[2].y+10, 0.6f, config->textColor(), "deleteFile", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "extractFile"))/2+150-70, creatorButtons[3].y+10, 0.6f, config->textColor(), "extractFile", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "installCia"))/2-150+70, creatorButtons[4].y+10, 0.6f, config->textColor(), "installCia", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "mkdir"))/2+150-70, creatorButtons[5].y+10, 0.6f, config->textColor(), "mkdir", 140);
	} else if (this->page == 1) {
		for (int i = 0; i < 6; i++) {
			if (this->Selection == i) {
				Gui::Draw_Rect(creatorButtons[i].x, creatorButtons[i].y, creatorButtons[i].w, creatorButtons[i].h, config->selectedColor());
			} else {
				Gui::Draw_Rect(creatorButtons[i].x, creatorButtons[i].y, creatorButtons[i].w, creatorButtons[i].h, config->unselectedColor());
			}
		}

		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "rmdir"))/2-150+70, creatorButtons[0].y+10, 0.6f, config->textColor(), "rmdir", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "mkfile"))/2+150-70, creatorButtons[1].y+10, 0.6f, config->textColor(), "mkfile", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "TimeMsg"))/2-150+70, creatorButtons[2].y+10, 0.6f, config->textColor(), "TimeMsg", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "saveConfig"))/2+150-70, creatorButtons[3].y+10, 0.6f, config->textColor(), "saveConfig", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "bootTitle"))/2-150+70, creatorButtons[4].y+10, 0.6f, config->textColor(), "bootTitle", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "promptMsg"))/2+150-70, creatorButtons[5].y+10, 0.6f, config->textColor(), "promptMsg", 140);

	} else if (this->page == 2) {
		for (int i = 0; i < 3; i++) {
			if (this->Selection == i) {
				Gui::Draw_Rect(creatorButtons[i].x, creatorButtons[i].y, creatorButtons[i].w, creatorButtons[i].h, config->selectedColor());
			} else {
				Gui::Draw_Rect(creatorButtons[i].x, creatorButtons[i].y, creatorButtons[i].w, creatorButtons[i].h, config->unselectedColor());
			}
		}

		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "copy"))/2-150+70, creatorButtons[0].y+10, 0.6f, config->textColor(), "copy", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "move"))/2+150-70, creatorButtons[1].y+10, 0.6f, config->textColor(), "move", 140);
		Gui::DrawString((320-Gui::GetStringWidth(0.6f, "change Entry"))/2-150+70, creatorButtons[2].y+10, 0.6f, config->textColor(), "change Entry", 140);
		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	}
}

void ScriptCreator::createNewJson(std::string fileName) {
	std::ofstream ofstream;
	ofstream.open(fileName.c_str(), std::ofstream::out | std::ofstream::app);
	ofstream << "{ }";
	ofstream.close();
}

// Test.
void ScriptCreator::createDownloadRelease() {
	// Repo.
	std::string repo = Input::setkbdString(50, "Enter the name of the Owner.");
	repo += "/";
	repo += Input::setkbdString(50, "Enter the name of the repo.");
	// File.
	std::string file = Input::setkbdString(50, "Enter the name of the file.");
	// Output.
	std::string output = Input::setkbdString(50, "Enter the name of the Output path.");
	// Prerelease.
	bool prerelease = true;
	// Message.
	std::string message = Input::setkbdString(50, "Enter the Message.");

	this->editScript[this->entryName].push_back({{"type", "downloadRelease"}, {"repo", repo}, {"file", file}, {"output", output}, {"includePrerelease", prerelease}, {"message", message}});
	Logging::writeToLog("Execute 'ScriptCreator::createDownloadRelease();'.");
}

// To-Do.

void ScriptCreator::createDownloadFile() {
	// URL of the file.
	std::string file = Input::setkbdString(50, "Enter the URL of the file.");
	// Output.
	std::string output = Input::setkbdString(50, "Enter the name of the Output path.");
	// Message.
	std::string message = Input::setkbdString(50, "Enter the Message.");

	this->editScript[this->entryName].push_back({{"type", "downloadFile"}, {"file", file}, {"output", output}, {"message", message}});
	Logging::writeToLog("Execute 'ScriptCreator::createDownloadFile();'.");
}


void ScriptCreator::createDeleteFile() {
	// URL of the file.
	std::string file = Input::setkbdString(50, "Enter the path to the file.");
	// Message.
	std::string message = Input::setkbdString(50, "Enter the Message.");

	this->editScript[this->entryName].push_back({{"type", "deleteFile"}, {"file", file}, {"message", message}});
	Logging::writeToLog("Execute 'ScriptCreator::createDeleteFile();'.");
}


void ScriptCreator::createExtractFile() {
	// File path.
	std::string file = Input::setkbdString(50, "Enter the path to the file.");
	// Input of the archive.
	std::string input = Input::setkbdString(50, "Enter the Input of what should be extracted.");
	// Output path.
	std::string output = Input::setkbdString(50, "Enter the output path.");
	// Message.
	std::string message = Input::setkbdString(50, "Enter the Message.");

	this->editScript[this->entryName].push_back({{"type", "extractFile"}, {"file", file}, {"input", input}, {"output", output}, {"message", message}});
	Logging::writeToLog("Execute 'ScriptCreator::createExtractFile();'.");
}


void ScriptCreator::createInstallCia() {
	// File path.
	std::string file = Input::setkbdString(50, "Enter the path to the CIA File.");
	// Message.
	std::string message = Input::setkbdString(50, "Enter the Message.");

	this->editScript[this->entryName].push_back({{"type", "installCia"}, {"file", file}, {"message", message}});
	Logging::writeToLog("Execute 'ScriptCreator::createInstallCia();'.");
}


void ScriptCreator::createMkDir() {
	// Directory path.
	std::string directory = Input::setkbdString(50, "Enter the directory path.");

	this->editScript[this->entryName].push_back({{"type", "mkdir"}, {"directory", directory}});
	Logging::writeToLog("Execute 'ScriptCreator::createMkDir();'.");
}

void ScriptCreator::createRmDir() {
	// Directory path.
	std::string directory = Input::setkbdString(50, "Enter the directory path.");

	this->editScript[this->entryName].push_back({{"type", "rmdir"}, {"directory", directory}});
	Logging::writeToLog("Execute 'ScriptCreator::createRmDir();'.");
}

void ScriptCreator::createMkFile() {
	// File path.
	std::string file = Input::setkbdString(50, "Enter the path to the new File.");

	this->editScript[this->entryName].push_back({{"type", "mkfile"}, {"file", file}});
	Logging::writeToLog("Execute 'ScriptCreator::createMkFile();'.");
}

void ScriptCreator::createTimeMsg() {
	int seconds = 0;
	// Message.
	std::string message = Input::setkbdString(50, "Enter the Message.");
	// Seconds.
	int temp = Input::setInt(999, "Enter the Seconds for the Message to display.");
	if (temp != -1)	seconds = temp;

	this->editScript[this->entryName].push_back({{"type", "timeMsg"}, {"message", message}, {"seconds", seconds}});
	Logging::writeToLog("Execute 'ScriptCreator::createTimeMsg();'.");
}

void ScriptCreator::createSaveConfig() {
	this->editScript[this->entryName].push_back({{"type", "saveConfig"}});
	Logging::writeToLog("Execute 'ScriptCreator::createSaveConfig();'.");
}

void ScriptCreator::createBootTitle() {
	std::string titleID = Input::setkbdString(50, "Enter the TitleID.");
	bool isNAND = Msg::promptMsg("Is the current title a NAND title?");
	std::string message = Input::setkbdString(50, "Enter the Message.");
	this->editScript[this->entryName].push_back({{"type", "bootTitle"}, {"TitleID", titleID}, {"NAND", isNAND}, {"message", message}});
	Logging::writeToLog("Execute 'ScriptCreator::createBootTitle();'.");
}

void ScriptCreator::createPromptMessage() {
	std::string message = Input::setkbdString(50, "Enter the Message.");
	this->editScript[this->entryName].push_back({{"type", "promptMessage"}, {"message", message}});
	Logging::writeToLog("Execute 'ScriptCreator::createPromptMessage();'.");
}

void ScriptCreator::createCopy() {
	std::string source = Input::setkbdString(50, "Enter the source location.");
	std::string destination = Input::setkbdString(50, "Enter the destination location.");
	std::string message = Input::setkbdString(50, "Enter the Message.");
	this->editScript[this->entryName].push_back({{"type", "copy"}, {"source", source}, {"destination", destination}, {"message", message}});
	Logging::writeToLog("Execute 'ScriptCreator::createCopy();'.");
}

void ScriptCreator::createMove() {
	std::string oldLocation = Input::setkbdString(50, "Enter the old location.");
	std::string newLocation = Input::setkbdString(50, "Enter the new location.");
	std::string message = Input::setkbdString(50, "Enter the Message.");
	this->editScript[this->entryName].push_back({{"type", "move"}, {"old", oldLocation}, {"new", newLocation}, {"message", message}});
	Logging::writeToLog("Execute 'ScriptCreator::createMove();'.");
}

void ScriptCreator::save() {
	FILE* file = fopen(this->jsonFileName.c_str(), "w");
	std::string result = this->editScript.dump(1, '\t');
	if (file)	fwrite(result.c_str(), 1, this->editScript.dump(1, '\t').size(), file);
	fclose(file);
	Logging::writeToLog("Execute 'ScriptCreator::save();'.");
}

// Important to make Scripts valid.
void ScriptCreator::setInfoStuff(void) {
	// Get needed things.
	const std::string test = Input::setkbdString(50, "Enter the Title of the script.");
	const std::string test2 = Input::setkbdString(50, "Enter the Author name of the script.");
	const std::string test3 = Input::setkbdString(80, "Enter the short description of the script.");
	const std::string test4 = Input::setkbdString(300, "Enter the long description of the script.");
	int scriptRevision = Input::setInt(99, "Enter the script revision.");
	// Set the real JSON stuff.
	this->setString("info", "title", test);
	this->setString("info", "author", test2);
	this->setString("info", "shortDesc", test3);
	this->setString("info", "description", test4);
	this->setInt("info", "version", SCRIPT_VERSION);
	if (scriptRevision != -1) {
		this->setInt("info", "revision", scriptRevision);
	} else {
		this->setInt("info", "revision", 1);
	}
}


void ScriptCreator::SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_B) {
		Gui::screenBack(config->screenFade());
		return;
	}

	if (hDown & KEY_A) {
		switch(Selection) {
			case 0:
				this->jsonFileName = config->scriptPath();
				this->jsonFileName += Input::setkbdString(20, "Enter the name of the JSON file.");
				if (this->jsonFileName != "") {
					this->jsonFileName += ".json";
					this->createNewJson(this->jsonFileName);
					this->openJson(this->jsonFileName);
					// If not included, create.
					if (!this->editScript.contains("info")) {
						this->setInfoStuff();
					}

					this->entryName = Input::setkbdString(50, "Enter the EntryName.");
					this->Selection = 0;
					this->mode = 1;
				}
				break;
			case 1:
				std::string tempScript = selectFilePath("Select the Script file.", config->scriptPath(), {"json"}, 2);
				if (tempScript != "") {
					this->jsonFileName = tempScript;
					if (access(this->jsonFileName.c_str(), F_OK) == 0) {
						this->openJson(this->jsonFileName);
						this->entryName = Input::setkbdString(50, "Enter the EntryName.");
						this->Selection = 0;
						this->mode = 1;
					}
				}
				break;
		}
	}


	if (hDown & KEY_UP) {
		if (this->Selection == 1)	this->Selection = 0;
	}

	if (hDown & KEY_DOWN) {
		if (this->Selection == 0)	this->Selection = 1;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			this->jsonFileName = config->scriptPath();
			this->jsonFileName += Input::setkbdString(20, "Enter the name of the JSON file.");
			if (this->jsonFileName != "") {
				this->jsonFileName += ".json";
				this->createNewJson(this->jsonFileName);
				this->openJson(this->jsonFileName);
				// If not included, create.
				if (!this->editScript.contains("info")) {
					this->setInfoStuff();
				}

				this->entryName = Input::setkbdString(50, "Enter the EntryName.");
				this->Selection = 0;
				this->mode = 1;
			}
		} else if (touching(touch, mainButtons[1])) {
			std::string tempScript = selectFilePath("Select the Script file.", config->scriptPath(), {"json"}, 2);
			if (tempScript != "") {
				this->jsonFileName = tempScript;
				if (access(this->jsonFileName.c_str(), F_OK) == 0) {
					this->openJson(this->jsonFileName);
					this->entryName = Input::setkbdString(50, "Enter the EntryName.");
					this->Selection = 0;
					this->mode = 1;
				}
			}
		}
	}
}

void ScriptCreator::scriptLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_B) {
		this->save();
		this->Selection = 0;
		this->mode = 0;
	}

	// Page 1.
	if (this->page == 0 || this->page == 1) {
		if (hDown & KEY_UP) {
			if (this->Selection > 1)	this->Selection -= 2;
		}

		if (hDown & KEY_DOWN) {
			if (this->Selection < 4)	this->Selection += 2;
		}

		if (hDown & KEY_LEFT) {
			if (this->Selection%2) this->Selection--;
		}

		if (hDown & KEY_RIGHT) {
			if (!(this->Selection%2)) this->Selection++;
		}

	} else if (this->page == 2) {
		if (hDown & KEY_RIGHT) {
			if (this->Selection == 0)	this->Selection = 1;
		}

		if (hDown & KEY_DOWN) {
			if (this->Selection == 0)	this->Selection = 2;
		}

		if (hDown & KEY_UP) {
			if (this->Selection == 2)	this->Selection = 0;
		}

		if (hDown & KEY_LEFT) {
			if (this->Selection == 1)	this->Selection = 0;
		}
	}

	
	if (hDown & KEY_R) {
		if (this->page < 2) {
			this->page++;
			this->Selection = 0;
		}
	}

	if (hDown & KEY_L) {
		if (this->page > 0) {
			this->page--;
			this->Selection = 0;
		}
	}

	if (hDown & KEY_A) {
		if (this->page == 0) {
			switch(this->Selection) {
				case 0:
					this->createDownloadRelease();
					break;
				case 1:
					this->createDownloadFile();
					break;
				case 2:
					this->createDeleteFile();
					break;
				case 3:
					this->createExtractFile();
					break;
				case 4:
					this->createInstallCia();
					break;
				case 5:
					this->createMkDir();
					break;
			}
		} else if (this->page == 1) {
			switch(this->Selection) {
				case 0:
					this->createRmDir();
					break;
				case 1:
					this->createMkFile();
					break;
				case 2:
					this->createTimeMsg();
					break;
				case 3:
					this->createSaveConfig();
					break;
				case 4:
					this->createBootTitle();
					break;
				case 5:
					this->createPromptMessage();
					break;
			}
		} else if (this->page == 2) {
			switch(this->Selection) {
				case 0:
					this->createCopy();
					break;
				case 1:
					this->createMove();
					break;
				case 2:
					this->entryName = Input::setkbdString(50, "Enter the new entry.");
					break;
			}
		}
	}

	if (hDown & KEY_TOUCH) {
		if (this->page == 0) {
			if (touching(touch, creatorButtons[0])) {
				this->createDownloadRelease();
			} else if (touching(touch, creatorButtons[1])) {
				this->createDownloadFile();
			} else if (touching(touch, creatorButtons[2])) {
				this->createDeleteFile();
			} else if (touching(touch, creatorButtons[3])) {
				this->createExtractFile();
			} else if (touching(touch, creatorButtons[4])) {
				this->createInstallCia();
			} else if (touching(touch, creatorButtons[5])) {
				this->createMkDir();
			}
		} else if (this->page == 1) {
			if (touching(touch, creatorButtons[0])) {
				this->createRmDir();
			} else if (touching(touch, creatorButtons[1])) {
				this->createMkFile();
			} else if (touching(touch, creatorButtons[2])) {
				this->createTimeMsg();
			} else if (touching(touch, creatorButtons[3])) {
				this->createSaveConfig();
			} else if (touching(touch, creatorButtons[4])) {
				this->createBootTitle();
			} else if (touching(touch, creatorButtons[5])) {
				this->createPromptMessage();
			}
		} else if (this->page == 2) {
			if (touching(touch, creatorButtons[0])) {
				this->createCopy();
			} else if (touching(touch, creatorButtons[1])) {
				this->createMove();
			} else if (touching(touch, creatorButtons[2])) {
				this->entryName = Input::setkbdString(50, "Enter the new entry.");
			}
		}
	}

	if (hDown & KEY_X) {
		this->setInfoStuff(); // Probably not needed at all.
	}
}

void ScriptCreator::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (this->mode == 0) {
		this->SubMenuLogic(hDown, hHeld, touch);
	} else if (this->mode == 1) {
		this->scriptLogic(hDown, hHeld, touch);
	}
}