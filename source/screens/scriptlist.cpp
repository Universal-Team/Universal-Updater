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

#include "download.hpp"
#include "mainMenu.hpp"
#include "scriptBrowse.hpp"
#include "scriptCreator.hpp"
#include "scriptlist.hpp"

#include <algorithm>
#include <fstream>
#include <regex>
#include <unistd.h>

extern std::unique_ptr<Config> config;
extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern bool checkWifiStatus(void);
extern void notImplemented(void);

bool isScriptSelected = false;
extern bool changesMade;

// Parse the script for the list.
ScriptInfo parseInfo(std::string fileName) {
	FILE* file = fopen(fileName.c_str(), "rt");
	if (!file) {
		printf("File not found.\n");
		return {"", ""};
	}

	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	ScriptInfo info;
	info.title = ScriptHelper::getString(json, "info", "title");
	info.author = ScriptHelper::getString(json, "info", "author");
	info.shortDesc = ScriptHelper::getString(json, "info", "shortDesc");
	return info;
}

// Check if Script version has the current version.
bool ScriptList::checkForValidate(void) {
	FILE* file = fopen(currentFile.c_str(), "rt");
	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
	int ver = ScriptHelper::getNum(json, "info", "version");
	if (ver == 3 || ver == 4) {
		return true;
	} else {
		Msg::DisplayWarnMsg(Lang::get("INCOMPATIBLE_SCRIPT"));
		return false;
	}

	return false;
}

// Open a script file and return it to a JSON.
nlohmann::json ScriptList::openScriptFile() {
	FILE* file = fopen(currentFile.c_str(), "rt");
	nlohmann::json jsonFile;
	if (file)	jsonFile = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
	return jsonFile;
}

// Parse the objects from a script.
std::vector<std::string> parseObjects(std::string fileName) {
	FILE* file = fopen(fileName.c_str(), "rt");
	if (!file) {
		printf("File not found.\n");
		return {{""}};
	}

	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	std::vector<std::string> objs;
	for(auto it = json.begin(); it != json.end(); it++) {
		if (it.key() != "info") {
			objs.push_back(it.key());
		}
	}

	return objs;
}

// Parse the description from the script.
std::string Description(nlohmann::json &json) {
	std::string out = "";
	if (json.at("info").contains("description")) out = json.at("info").at("description");
	else out = "";
	return out;
}


// Return the color for the script.
u32 getColor(std::string colorString) {
	if (colorString.length() < 7 || std::regex_search(colorString.substr(1), std::regex("[^0-9a-f]"))) { // invalid color
		return 0;
	}

	int r = std::stoi(colorString.substr(1, 2), nullptr, 16);
	int g = std::stoi(colorString.substr(3, 2), nullptr, 16);
	int b = std::stoi(colorString.substr(5, 2), nullptr, 16);
	return RGBA8(r, g, b, 0xFF);
}

// Color listing!
u32 barColor, bgTopColor, bgBottomColor, TextColor, selected, unselected, progressBar;

// Load the colors from the script.
void loadColors(nlohmann::json &json) {
	if (config->useScriptColor()) {
		u32 colorTemp;
		colorTemp = getColor(ScriptHelper::getString(json, "info", "barColor"));
		barColor = colorTemp == 0 ? config->barColor() : colorTemp;

		colorTemp = getColor(ScriptHelper::getString(json, "info", "bgTopColor"));
		bgTopColor = colorTemp == 0 ? config->topBG() : colorTemp;

		colorTemp = getColor(ScriptHelper::getString(json, "info", "bgBottomColor"));
		bgBottomColor = colorTemp == 0 ? config->bottomBG() : colorTemp;

		colorTemp = getColor(ScriptHelper::getString(json, "info", "textColor"));
		TextColor = colorTemp == 0 ? config->textColor() : colorTemp;

		colorTemp = getColor(ScriptHelper::getString(json, "info", "selectedColor"));
		selected = colorTemp == 0 ? config->selectedColor() : colorTemp;

		colorTemp = getColor(ScriptHelper::getString(json, "info", "unselectedColor"));
		unselected = colorTemp == 0 ? config->unselectedColor() : colorTemp;

		colorTemp = getColor(ScriptHelper::getString(json, "info", "progressbarColor"));
		progressBar = colorTemp == 0 ? config->progressbarColor() : colorTemp;
	} else {
		barColor = config->barColor();
		bgTopColor = config->topBG();
		bgBottomColor = config->bottomBG();
		TextColor = config->textColor();
		selected = config->selectedColor();
		unselected = config->unselectedColor();
		progressBar = config->progressbarColor();
	}
}

void ScriptList::DrawSubMenu(void) const {
	GFX::DrawTop();
	Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.7f, config->textColor(), Lang::get("SCRIPTS_SUBMENU"), 400);

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	GFX::DrawButton(subPos[0].x, subPos[0].y, Lang::get("SCRIPTLIST"));
	GFX::DrawButton(subPos[1].x, subPos[1].y, Lang::get("GET_SCRIPTS"));
	GFX::DrawButton(subPos[2].x, subPos[2].y, Lang::get("SCRIPTCREATOR"));
	GFX::DrawButton(subPos[3].x, subPos[3].y, Lang::get("CHANGE_SCRIPTPATH"));

	// Selector.
	Animation::Button(subPos[Selection].x, subPos[Selection].y, .060);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}

// Load the description.
void ScriptList::loadDesc(void) {
	lines.clear();
	while(Desc.find('\n') != Desc.npos) {
		lines.push_back(Desc.substr(0, Desc.find('\n')));
		Desc = Desc.substr(Desc.find('\n')+1);
	}

	lines.push_back(Desc.substr(0, Desc.find('\n')));
}

// MUST be included there and not in the Header.
extern int AutobootWhat;
bool changeBackHandle = false;

ScriptList::ScriptList() {
	if (AutobootWhat == 2) {
		// If Script isn't found, go to MainMenu.
		if (access(config->autobootFile().c_str(), F_OK) != 0) {
			AutobootWhat = 0;
			changeBackHandle = true;
			Gui::setScreen(std::make_unique<MainMenu>(), config->screenFade(), true);
		}

		if (ScriptHelper::checkIfValid(config->autobootFile()) == true) {
			ScriptInfo fI = parseInfo(config->autobootFile());
			currentFile = config->autobootFile();
			if (checkForValidate()) {
				selectedTitle = fI.title;
				jsonFile = openScriptFile();
				Desc = Description(jsonFile);
				fileInfo2 = parseObjects(currentFile);
				loadColors(jsonFile);
				loadDesc();
				isScriptSelected = true;
				Selection = 0;
				mode = 2;
				changeBackHandle = true;
				AutobootWhat = 0;
			} else {
				AutobootWhat = 0;
				changeBackHandle = true;
				Gui::setScreen(std::make_unique<MainMenu>(), config->screenFade(), true);
			}
		} else {
			AutobootWhat = 0;
			changeBackHandle = true;
			Gui::setScreen(std::make_unique<MainMenu>(), config->screenFade(), true);
		}
	}
}

void ScriptList::DrawList(void) const {
	std::string line1;
	std::string line2;
	std::string scriptAmount = std::to_string(Selection +1) + " | " + std::to_string(fileInfo.size());
	GFX::DrawTop();
	Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.7f, config->textColor(), "Universal-Updater", 400);
	Gui::DrawString(397-Gui::GetStringWidth(0.6f, scriptAmount), (config->useBars() ? 239 : 237)-Gui::GetStringHeight(0.6f, scriptAmount), 0.6f, config->textColor(), scriptAmount);

	Gui::DrawStringCentered(0, 80, 0.7f, config->textColor(), Lang::get("TITLE") + std::string(fileInfo[Selection].title), 400);
	Gui::DrawStringCentered(0, 100, 0.7f, config->textColor(), Lang::get("AUTHOR") + std::string(fileInfo[Selection].author), 400);
	Gui::DrawStringCentered(0, 120, 0.6f, config->textColor(), std::string(fileInfo[Selection].shortDesc), 400);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();
	GFX::DrawArrow(295, -1);
	GFX::DrawArrow(315, 240, 180.0);
	GFX::DrawArrow(0, 218, 0, 1);
	GFX::DrawSpriteBlend(sprites_dropdown_idx, arrowPos[3].x, arrowPos[3].y);

	if (config->viewMode() == 0) {
		for(int i = 0; i < ENTRIES_PER_SCREEN && i < (int)fileInfo.size(); i++) {
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, config->unselectedColor());
			line1 = fileInfo[screenPos + i].title;
			line2 = fileInfo[screenPos + i].author;
			if (screenPos + i == Selection) {
				if (!dropDownMenu) {
					Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, config->selectedColor());
				}
			}
			Gui::DrawStringCentered(0, 38+(i*57), 0.7f, config->textColor(), line1, 320);
			Gui::DrawStringCentered(0, 62+(i*57), 0.7f, config->textColor(), line2, 320);
		}
	} else if (config->viewMode() == 1) {
		for(int i = 0; i < ENTRIES_PER_LIST && i < (int)fileInfo.size();i++) {
			Gui::Draw_Rect(0, (i+1)*27, 320, 25, config->unselectedColor());
			line1 = fileInfo[screenPosList + i].title;
			if (screenPosList + i == Selection) {
				if (!dropDownMenu) {
					Gui::drawAnimatedSelector(0, (i+1)*27, 320, 25, .060, TRANSPARENT, config->selectedColor());
				}
			}
			Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, config->textColor(), line1, 320);
		}
	}

	// DropDown Menu.
	if (dropDownMenu) {
		// Draw Operation Box.
		Gui::Draw_Rect(0, 25, 140, 87, config->barColor());
		for (int i = 0; i < 2; i++) {
			if (dropSelection == i) {
				Gui::drawAnimatedSelector(dropPos2[i].x, dropPos2[i].y, dropPos2[i].w, dropPos2[i].h, .090, TRANSPARENT, config->selectedColor());
			} else {
				Gui::Draw_Rect(dropPos2[i].x, dropPos2[i].y, dropPos2[i].w, dropPos2[i].h, config->unselectedColor());
			}
		}
		// Draw Dropdown Icons.
		GFX::DrawSpriteBlend(sprites_delete_idx, dropPos[0].x, dropPos[0].y);
		GFX::DrawSpriteBlend(sprites_view_idx, dropPos[1].x, dropPos[1].y);
		// Dropdown Text.
		Gui::DrawString(dropPos[0].x+30, dropPos[0].y+5, 0.4f, config->textColor(), Lang::get("DELETE_DDM"), 100);
		Gui::DrawString(dropPos[1].x+30, dropPos[1].y+5, 0.4f, config->textColor(), Lang::get("VIEW_DDM"), 100);
	}
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}

void ScriptList::Draw(void) const {
	if (mode == 0) {
		DrawSubMenu();
	} else if (mode == 1) {
		DrawList();
	} else if (mode == 2) {
		DrawSingleObject();
	} else if (mode == 3) {
		DrawGlossary();
	}
}

void ScriptList::DrawSingleObject(void) const {
	std::string info;
	std::string entryAmount = std::to_string(Selection+1) + " | " + std::to_string(fileInfo2.size());
	GFX::DrawTop();
	Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.7f, TextColor, selectedTitle, 400);
	Gui::DrawString(397-Gui::GetStringWidth(0.6f, entryAmount), (config->useBars() ? 239 : 237)-Gui::GetStringHeight(0.6f, entryAmount), 0.6f, TextColor, entryAmount);

	for(uint i = 0; i < lines.size(); i++) {
		Gui::DrawStringCentered(0, 120-((lines.size()*20)/2)+i*20, 0.6f, TextColor, lines[i], 400);
	}

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();
	GFX::DrawArrow(295, -1);
	GFX::DrawArrow(315, 240, 180.0);
	GFX::DrawArrow(0, 218, 0, 1);
	GFX::DrawSpriteBlend(sprites_dropdown_idx, arrowPos[3].x, arrowPos[3].y);

	if (config->viewMode() == 0) {
		for(int i = 0; i < ENTRIES_PER_SCREEN && i < (int)fileInfo2.size(); i++) {
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, unselected);
			info = fileInfo2[screenPos + i];
			if (screenPos + i == Selection) {
				if (!dropDownMenu) {
					Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, selected);
				}
			}
			Gui::DrawStringCentered(0, 50+(i*57), 0.7f, TextColor, info, 320);
		}

	} else if (config->viewMode() == 1) {
		for(int i = 0; i < ENTRIES_PER_LIST && i < (int)fileInfo2.size(); i++) {
			Gui::Draw_Rect(0, (i+1)*27, 320, 25, unselected);
			info = fileInfo2[screenPosList + i];
			if (screenPosList + i == Selection) {
				if (!dropDownMenu) {
					Gui::drawAnimatedSelector(0, (i+1)*27, 320, 25, .060, TRANSPARENT, selected);
				}
			}
			Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, TextColor, info, 320);
		}
	}

	// DropDown Menu.
	if (dropDownMenu) {
		// Draw Operation Box.
		Gui::Draw_Rect(0, 25, 140, 44, barColor);
		Gui::drawAnimatedSelector(dropPos2[0].x, dropPos2[0].y, dropPos2[0].w, dropPos2[0].h, .090, TRANSPARENT, selected);
		// Draw Dropdown Icons.
		GFX::DrawSpriteBlend(sprites_view_idx, dropPos[0].x, dropPos[0].y);
		// Dropdown Text.
		Gui::DrawString(dropPos[0].x+30, dropPos[0].y+5, 0.4f, TextColor, Lang::get("VIEW_DDM"), 100);
	}

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}

void ScriptList::refreshList() {
	if (returnIfExist(config->scriptPath(), {"json"}) == true) {
		Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
		dirContents.clear();
		fileInfo.clear();
		chdir(config->scriptPath().c_str());
		getDirectoryContents(dirContents, {"json"});
		for(uint i = 0; i < dirContents.size(); i++) {
			fileInfo.push_back(parseInfo(dirContents[i].name));
		}

		Selection = 0;
		mode = 1;
	} else {
		Msg::DisplayWarnMsg(Lang::get("GET_SCRIPTS_FIRST"));
		Selection = 0;
		mode = 0;
	}
}

void ScriptList::SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		if (changeBackHandle) {
			Gui::setScreen(std::make_unique<MainMenu>(), config->screenFade(), true);
		} else {
			Gui::screenBack(config->screenFade());
			return;
		}
	}

	// Navigation.
	if (hDown & KEY_UP) {
		if (Selection > 1)	Selection -= 2;
	} else if (hDown & KEY_DOWN) {
		if (Selection < 3 && Selection != 2 && Selection != 3)	Selection += 2;
	} else if (hDown & KEY_LEFT) {
		if (Selection%2) Selection--;
	} else if (hDown & KEY_RIGHT) {
		if (!(Selection%2)) Selection++;
	}

	if (hDown & KEY_A) {
		switch(Selection) {
			case 0:
				if (returnIfExist(config->scriptPath(), {"json"}) == true) {
					Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
					dirContents.clear();
					chdir(config->scriptPath().c_str());
					getDirectoryContents(dirContents, {"json"});
					for(uint i=0;i<dirContents.size();i++) {
						fileInfo.push_back(parseInfo(dirContents[i].name));
					}
					mode = 1;
				} else {
					Msg::DisplayWarnMsg(Lang::get("GET_SCRIPTS_FIRST"));
				}
				break;
			case 1:
				if (checkWifiStatus() == true) {
					Gui::setScreen(std::make_unique<ScriptBrowse>(), config->screenFade(), true);
				} else {
					notConnectedMsg();
				}
				break;
			case 2:
				if (isTesting == true) {
					Gui::setScreen(std::make_unique<ScriptCreator>(), config->screenFade(), true);
				} else {
					notImplemented();
				}
				break;
			case 3:
				std::string tempScript = selectFilePath(Lang::get("SELECT_SCRIPT_PATH"), config->scriptPath(), {});
				if (tempScript != "") {
					config->scriptPath(tempScript);
					changesMade = true;
				}
				break;
		}
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, subPos[0])) {
			if (returnIfExist(config->scriptPath(), {"json"}) == true) {
				Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
				dirContents.clear();
				chdir(config->scriptPath().c_str());
				getDirectoryContents(dirContents, {"json"});
				for(uint i = 0; i < dirContents.size(); i++) {
					fileInfo.push_back(parseInfo(dirContents[i].name));
				}
				mode = 1;
			} else {
				Msg::DisplayWarnMsg(Lang::get("GET_SCRIPTS_FIRST"));
			}
		} else if (touching(touch, subPos[1])) {
			if (checkWifiStatus() == true) {
				Gui::setScreen(std::make_unique<ScriptBrowse>(), config->screenFade(), true);
			} else {
				notConnectedMsg();
			}
		} else if (touching(touch, subPos[2])) {
			if (isTesting == true) {
				Gui::setScreen(std::make_unique<ScriptCreator>(), config->screenFade(), true);
			} else {
				notImplemented();
			}
		} else if (touching(touch, subPos[3])) {
			std::string tempScript = selectFilePath(Lang::get("SELECT_SCRIPT_PATH"), config->scriptPath(), {});
			if (tempScript != "") {
				config->scriptPath(tempScript);
				changesMade = true;
			}
		}
	}
}

void ScriptList::deleteScript(int selectedScript) {
	std::string path = config->scriptPath();
	path += dirContents[selectedScript].name;
	deleteFile(path.c_str());
	// Refresh the list.
	Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
	Selection = 0;
	dirContents.clear();
	fileInfo.clear();
	chdir(config->scriptPath().c_str());
	getDirectoryContents(dirContents, {"json"});
	for(uint i = 0; i < dirContents.size(); i++) {
		fileInfo.push_back(parseInfo(dirContents[i].name));
	}

	if (dirContents.size() == 0) {
		dirContents.clear();
		fileInfo.clear();
		mode = 0;
	}
}

void ScriptList::ListSelection(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;

	//DropDown Logic.
	if (dropDownMenu) {
		if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[3]))) {
			dropDownMenu = false;
		}
		if (hDown & KEY_DOWN) {
			if (dropSelection < 1)	dropSelection++;
		}
		if (hDown & KEY_UP) {
			if (dropSelection > 0)	dropSelection--;
		}
		if (hDown & KEY_A) {
			switch(dropSelection) {
				case 0:
					if (Msg::promptMsg(Lang::get("DELETE_SCRIPT"))) {
						deleteScript(Selection);
					}
					break;
				case 1:
					if (config->viewMode() == 0) {
						config->viewMode(1);
					} else {
						config->viewMode(0);
					}
					break;
			}
			dropDownMenu = false;
		}

		if (hDown & KEY_TOUCH) {
			if (touching(touch, dropPos2[0])) {
				if (Msg::promptMsg(Lang::get("DELETE_SCRIPT"))) {
					deleteScript(Selection);
				}
				dropDownMenu = false;
			} else if (touching(touch, dropPos2[1])) {
				if (config->viewMode() == 0) {
					config->viewMode(1);
				} else {
					config->viewMode(0);
				}
				dropDownMenu = false;
			}
		}
	} else {
		if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
			fileInfo.clear();
			Selection = 0;
			mode = 0;
		}

		if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[3]))) {
			dropSelection = 0;
			dropDownMenu = true;
		}

		if (hDown & KEY_START) {
			if (config->autoboot() == 2) {
				if (Msg::promptMsg(Lang::get("DISABLE_AUTOBOOT"))) {
					config->autoboot(0);
					config->autobootFile("");
					changesMade = true;
				}
			} else {
				if (dirContents[Selection].isDirectory) {
				} else if (fileInfo.size() != 0) {
					if (ScriptHelper::checkIfValid(dirContents[Selection].name) == true) {
						if (Msg::promptMsg(Lang::get("AUTOBOOT_SCRIPT"))) {
							config->autobootFile(config->scriptPath() + dirContents[Selection].name);
							config->autoboot(2);
							changesMade = true;
						}
					}
				}
			}
		}

		if ((hHeld & KEY_DOWN && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[1]))) {
			if (Selection < (int)fileInfo.size()-1) {
				Selection++;
			} else {
				Selection = 0;
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if ((hHeld & KEY_UP && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[0]))) {
			if (Selection > 0) {
				Selection--;
			} else {
				Selection = (int)fileInfo.size()-1;
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if ((hHeld & KEY_RIGHT && !keyRepeatDelay)) {
			if (config->viewMode() == 0) {
				if (Selection < (int)fileInfo.size()-1-3) {
					Selection += 3;
				} else {
					Selection = (int)fileInfo.size()-1;
				}
			} else {
				if (Selection < (int)fileInfo.size()-1-6) {
					Selection += 7;
				} else {
					Selection = (int)fileInfo.size()-1;
				}
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if ((hHeld & KEY_LEFT && !keyRepeatDelay)) {
			if (config->viewMode() == 0) {
				if (Selection > 2) {
					Selection -= 3;
				} else {
					Selection = 0;
				}
			} else {
				if (Selection > 6) {
					Selection -= 7;
				} else {
					Selection = 0;
				}
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if (hDown & KEY_TOUCH) {
			if (config->viewMode() == 0) {
				for(int i = 0; i < ENTRIES_PER_SCREEN && i < (int)fileInfo.size(); i++) {
					if (touch.py > 40+(i*57) && touch.py < 40+(i*57)+45) {
						if (!dirContents[screenPos + i].isDirectory && fileInfo.size() != 0) {
							if (ScriptHelper::checkIfValid(dirContents[screenPos + i].name) == true) {
								currentFile = dirContents[screenPos + i].name;
								if (checkForValidate()) {
									selectedTitle = fileInfo[screenPos + i].title;
									jsonFile = openScriptFile();
									Desc = Description(jsonFile);
									checkForValidate();
									fileInfo2 = parseObjects(currentFile);
									loadColors(jsonFile);
									loadDesc();
									isScriptSelected = true;
									Selection = 0;
									mode = 2;
								}
							}
						}
					}
				}
			} else if (config->viewMode() == 1) {
				for(int i = 0; i < ENTRIES_PER_LIST && i < (int)fileInfo.size(); i++) {
					if (touch.py > (i+1)*27 && touch.py < (i+2)*27) {
						if (!dirContents[screenPosList + i].isDirectory && fileInfo.size() != 0) {
							if (ScriptHelper::checkIfValid(dirContents[screenPosList + i].name) == true) {
								currentFile = dirContents[screenPosList + i].name;
								if (checkForValidate()) {
									selectedTitle = fileInfo[screenPosList + i].title;
									jsonFile = openScriptFile();
									Desc = Description(jsonFile);
									checkForValidate();
									fileInfo2 = parseObjects(currentFile);
									loadColors(jsonFile);
									loadDesc();
									isScriptSelected = true;
									Selection = 0;
									mode = 2;
								}
							}
						}
					}
				}
			}
		}

		if (hDown & KEY_A) {
			if (dirContents[Selection].isDirectory) {
			} else if (fileInfo.size() != 0) {
				if (ScriptHelper::checkIfValid(dirContents[Selection].name) == true) {
					currentFile = dirContents[Selection].name;
					if (checkForValidate()) {
						selectedTitle = fileInfo[Selection].title;
						jsonFile = openScriptFile();
						Desc = Description(jsonFile);
						checkForValidate();
						fileInfo2 = parseObjects(currentFile);
						loadColors(jsonFile);
						loadDesc();
						isScriptSelected = true;
						Selection = 0;
						mode = 2;
					}
				}
			}
		}

		if (config->viewMode() == 0) {
			if (Selection < screenPos) {
				screenPos = Selection;
			} else if (Selection > screenPos + ENTRIES_PER_SCREEN - 1) {
				screenPos = Selection - ENTRIES_PER_SCREEN + 1;
			}
		} else if (config->viewMode() == 1) {
			if (Selection < screenPosList) {
				screenPosList = Selection;
			} else if (Selection > screenPosList + ENTRIES_PER_LIST - 1) {
				screenPosList = Selection - ENTRIES_PER_LIST + 1;
			}
		}
	}
}

void ScriptList::SelectFunction(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;

	//DropDown Logic.
	if (dropDownMenu) {
		if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[3]))) {
			dropDownMenu = false;
		}

		if (hDown & KEY_A) {
			if (config->viewMode() == 0) {
				config->viewMode(1);
			} else {
				config->viewMode(0);
			}
			dropDownMenu = false;
		}

		if (hDown & KEY_TOUCH) {
			if (touching(touch, dropPos2[0])) {
				if (config->viewMode() == 0) {
					config->viewMode(1);
				} else {
					config->viewMode(0);
				}
				dropDownMenu = false;
			}
		}
	} else {
		if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[3]))) {
			dropSelection = 0;
			dropDownMenu = true;
		}

		if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
			Selection = 0;
			fileInfo2.clear();
			isScriptSelected = false;
			refreshList();
		}

		if ((hHeld & KEY_DOWN && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[1]))) {
			if (Selection < (int)fileInfo2.size()-1) {
				Selection++;
			} else {
				Selection = 0;
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if ((hHeld & KEY_UP && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[0]))) {
			if (Selection > 0) {
				Selection--;
			} else {
				Selection = (int)fileInfo2.size()-1;
			}
			
			keyRepeatDelay = config->keyDelay();
		}


		if ((hHeld & KEY_RIGHT && !keyRepeatDelay)) {
			if (config->viewMode() == 0) {
				if (Selection < (int)fileInfo2.size()-1-3) {
					Selection += 3;
				} else {
					Selection = (int)fileInfo2.size()-1;
				}
			} else {
				if (Selection < (int)fileInfo2.size()-1-6) {
					Selection += 7;
				} else {
					Selection = (int)fileInfo2.size()-1;
				}
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if ((hHeld & KEY_LEFT && !keyRepeatDelay)) {
			if (config->viewMode() == 0) {
				if (Selection > 2) {
					Selection -= 3;
				} else {
					Selection = 0;
				}
			} else {
				if (Selection > 6) {
					Selection -= 7;
				} else {
					Selection = 0;
				}
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if (hDown & KEY_TOUCH) {
			if (config->viewMode() == 0) {
				for(int i = 0; i < ENTRIES_PER_SCREEN && i < (int)fileInfo2.size(); i++) {
					if (touch.py > 40+(i*57) && touch.py < 40+(i*57)+45) {
						if (fileInfo2.size() != 0) {
							choice = fileInfo2[screenPos + i];
							if (Msg::promptMsg(Lang::get("EXECUTE_SCRIPT") + "\n\n" + choice)) {
								runFunctions(jsonFile);
							}
						}
					}
				}
			} else if (config->viewMode() == 1) {
				for(int i = 0; i < ENTRIES_PER_LIST && i < (int)fileInfo2.size(); i++) {
					if (touch.py > (i+1)*27 && touch.py < (i+2)*27) {
						if (fileInfo2.size() != 0) {
							choice = fileInfo2[screenPosList + i];
							if (Msg::promptMsg(Lang::get("EXECUTE_SCRIPT") + "\n\n" + choice)) {
								runFunctions(jsonFile);
							}
						}
					}
				}
			}
		}

		if (hDown & KEY_A) {
			if (fileInfo2.size() != 0) {
				choice = fileInfo2[Selection];
				if (Msg::promptMsg(Lang::get("EXECUTE_SCRIPT") + "\n\n" + choice)) {
					runFunctions(jsonFile);
				}
			}
		}
	
		if (hDown & KEY_SELECT) {
			config->barColor(barColor);
			config->topBG(bgTopColor);
			config->bottomBG(bgBottomColor);
			config->textColor(TextColor);
			config->selectedColor(selected);
			config->unselectedColor(unselected);
			changesMade = true;
		}

		if (config->viewMode() == 0) {
			if (Selection < screenPos) {
				screenPos = Selection;
			} else if (Selection > screenPos + ENTRIES_PER_SCREEN - 1) {
				screenPos = Selection - ENTRIES_PER_SCREEN + 1;
			}
		} else if (config->viewMode() == 1) {
			if (Selection < screenPosList) {
				screenPosList = Selection;
			} else if (Selection > screenPosList + ENTRIES_PER_LIST - 1) {
				screenPosList = Selection - ENTRIES_PER_LIST + 1;
			}
		}
	}
}

void ScriptList::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (mode == 0) {
		SubMenuLogic(hDown, hHeld, touch);
	} else if (mode == 1) {
		ListSelection(hDown, hHeld, touch);
	} else if (mode == 2) {
		SelectFunction(hDown, hHeld, touch);
	}

	if (hDown & KEY_L || hDown & KEY_R) {
		if (mode == 3) {
			mode = lastMode;
		} else if (mode == 1) {
			mode = 3;
			lastMode = 1; // To go back after it.
		} else if (mode == 2) {
			mode = 3;
			lastMode = 2;
		}
	}
}

void ScriptList::DrawGlossary(void) const {
	GFX::DrawTop();
	if (config->useBars() == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, config->textColor(), Lang::get("GLOSSARY"), 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, config->textColor(), Lang::get("GLOSSARY"), 400);
	}

	if (lastMode == 1) {
		Gui::DrawString(15, 35, 0.7f, config->textColor(), std::to_string(Selection +1) + " | " + std::to_string(fileInfo.size()), 40);
		Gui::DrawString(65, 35, 0.7f, config->textColor(), Lang::get("ENTRY"), 300);
	} else if (lastMode == 2) {
		Gui::DrawString(15, 35, 0.7f, config->textColor(), std::to_string(Selection+1) + " | " + std::to_string(fileInfo2.size()), 40);
		Gui::DrawString(65, 35, 0.7f, config->textColor(), Lang::get("ENTRY"), 300);
	}
	GFX::DrawBottom();

	GFX::DrawSpriteBlend(sprites_view_idx, 20, 40);
	Gui::DrawString(50, 42, 0.6f, config->textColor(), Lang::get("CHANGE_VIEW_MODE"), 260);

	GFX::DrawArrow(20, 70);
	Gui::DrawString(50, 72, 0.6f, config->textColor(), Lang::get("ENTRY_UP"), 260);
	GFX::DrawArrow(42, 125, 180.0);
	Gui::DrawString(50, 102, 0.6f, config->textColor(), Lang::get("ENTRY_DOWN"), 260);
	GFX::DrawArrow(20, 130, 0, 1);
	Gui::DrawString(50, 132, 0.6f, config->textColor(), Lang::get("GO_BACK"), 260);
	if (lastMode == 1) {
		GFX::DrawSpriteBlend(sprites_delete_idx, 20, 160);
		Gui::DrawString(50, 162, 0.6f, config->textColor(), Lang::get("DELETE_SCRIPT2"), 260);
	}

	GFX::DrawArrow(0, 218, 0, 1);
}

// Execute | run the script.
Result ScriptList::runFunctions(nlohmann::json &json) {
	Result ret = NONE; // No Error as of yet.
	for(int i = 0; i < (int)json.at(choice).size(); i++) {
		if (ret == NONE) {
			std::string type = json.at(choice).at(i).at("type");

			if (type == "deleteFile") {
				bool missing = false;
				std::string file, message;
				if (json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
				else	missing = true;
				if (json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
				if (!missing)	ret = ScriptHelper::removeFile(file, message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "downloadFile") {
				bool missing = false;
				std::string file, output, message;
				if (json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
				else	missing = true;
				if (json.at(choice).at(i).contains("output"))	output = json.at(choice).at(i).at("output");
				else	missing = true;
				if (json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
				if (!missing)	ret = ScriptHelper::downloadFile(file, output, message);
				else	ret = SYNTAX_ERROR;
		
			} else if (type == "downloadRelease") {
				bool missing = false, includePrereleases = false, showVersions = false;
				std::string repo, file, output, message;
				if (json.at(choice).at(i).contains("repo"))	repo = json.at(choice).at(i).at("repo");
				else	missing = true;
				if (json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
				else	missing = true;
				if (json.at(choice).at(i).contains("output"))	output = json.at(choice).at(i).at("output");
				else	missing = true;
				if (json.at(choice).at(i).contains("includePrereleases") && json.at(choice).at(i).at("includePrereleases").is_boolean())
					includePrereleases = json.at(choice).at(i).at("includePrereleases");
				if (json.at(choice).at(i).contains("showVersions") && json.at(choice).at(i).at("showVersions").is_boolean())
					showVersions = json.at(choice).at(i).at("showVersions");
				if (json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
				if (!missing)	ret = ScriptHelper::downloadRelease(repo, file, output, includePrereleases, showVersions, message);

			} else if (type == "extractFile") {
				bool missing = false;
				std::string file, input, output, message;
				if (json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
				else	missing = true;
				if (json.at(choice).at(i).contains("input"))	input = json.at(choice).at(i).at("input");
				else	missing = true;
				if (json.at(choice).at(i).contains("output"))	output = json.at(choice).at(i).at("output");
				else	missing = true;
				if (json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
				if (!missing)	ScriptHelper::extractFile(file, input, output, message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "installCia") {
				bool missing = false, updateSelf = false;
				std::string file, message;
				if (json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
				else	missing = true;
				if (json.at(choice).at(i).contains("updateSelf") && json.at(choice).at(i).at("updateSelf").is_boolean()) {
					updateSelf = json.at(choice).at(i).at("updateSelf");
				}
				if (json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
				if (!missing)	ScriptHelper::installFile(file, updateSelf, message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "mkdir") {
				bool missing = false;
				std::string directory, message;
				if (json.at(choice).at(i).contains("directory"))	directory = json.at(choice).at(i).at("directory");
				else	missing = true;
				if (!missing)	makeDirs(directory.c_str());
				else	ret = SYNTAX_ERROR;

			} else if (type == "rmdir") {
				bool missing = false;
				std::string directory, message, promptmsg;
				if (json.at(choice).at(i).contains("directory"))	directory = json.at(choice).at(i).at("directory");
				else	missing = true;
				promptmsg = Lang::get("DELETE_PROMPT") + "\n" + directory;
				if (!missing) {
					if (access(directory.c_str(), F_OK) != 0 ) {
						ret = DELETE_ERROR;
					} else {
						if (Msg::promptMsg(promptmsg)) {
							removeDirRecursive(directory.c_str());
						}
					}
				}
				else	ret = SYNTAX_ERROR;

			} else if (type == "mkfile") {
				bool missing = false;
				std::string file;
				if (json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
				else	missing = true;
				if (!missing)	ScriptHelper::createFile(file.c_str());
				else	ret = SYNTAX_ERROR;

			} else if (type == "timeMsg") {
				bool missing = false;
				std::string message;
				int seconds;
				if (json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
				else	missing = true;
				if (json.at(choice).at(i).contains("seconds") && json.at(choice).at(i).at("seconds").is_number())
				seconds = json.at(choice).at(i).at("seconds");
				else	missing = true;
				if (!missing)	ScriptHelper::displayTimeMsg(message, seconds);
				else	ret = SYNTAX_ERROR;

			} else if (type == "saveConfig") {
				config->save();

			} else if (type == "bootTitle") {
				std::string TitleID = "";
				std::string message = "";
				bool isNAND = false, missing = false;
				if (json.at(choice).at(i).contains("TitleID"))	TitleID = json.at(choice).at(i).at("TitleID");
				else	missing = true;
				if (json.at(choice).at(i).contains("NAND") && json.at(choice).at(i).at("NAND").is_boolean())	isNAND = json.at(choice).at(i).at("NAND");
				else	missing = true;
				if (json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
				else	missing = true;
				if (!missing)	ScriptHelper::bootTitle(TitleID, isNAND, message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "promptMessage") {
				std::string Message = "";
				if (json.at(choice).at(i).contains("message"))	Message = json.at(choice).at(i).at("message");
				ret = ScriptHelper::prompt(Message);
				
			} else if (type == "copy") {
				std::string Message = "", source = "", destination = "";
				bool missing = false;
				if (json.at(choice).at(i).contains("source"))	source = json.at(choice).at(i).at("source");
				else	missing = true;
				if (json.at(choice).at(i).contains("destination"))	destination = json.at(choice).at(i).at("destination");
				else	missing = true;
				if (json.at(choice).at(i).contains("message"))	Message = json.at(choice).at(i).at("message");
				if (!missing)	ret = ScriptHelper::copyFile(source, destination, Message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "move") {
				std::string Message = "", oldFile = "", newFile = "";
				bool missing = false;
				if (json.at(choice).at(i).contains("old"))	oldFile = json.at(choice).at(i).at("old");
				else	missing = true;
				if (json.at(choice).at(i).contains("new"))	newFile = json.at(choice).at(i).at("new");
				else	missing = true;
				if (json.at(choice).at(i).contains("message"))	Message = json.at(choice).at(i).at("message");
				if (!missing)	ret = ScriptHelper::renameFile(oldFile, newFile, Message);
				else	ret = SYNTAX_ERROR;
			}
		}
	}
	if (ret == NONE)	doneMsg();
	else if (ret == FAILED_DOWNLOAD)	Msg::DisplayWarnMsg(Lang::get("DOWNLOAD_ERROR"));
	else if (ret == SCRIPT_CANCELED)	Msg::DisplayWarnMsg(Lang::get("SCRIPT_CANCELED"));
	else if (ret == SYNTAX_ERROR)		Msg::DisplayWarnMsg(Lang::get("SYNTAX_ERROR"));
	else if (ret == COPY_ERROR)			Msg::DisplayWarnMsg(Lang::get("COPY_ERROR"));
	else if (ret == MOVE_ERROR)			Msg::DisplayWarnMsg(Lang::get("MOVE_ERROR"));
	else if (ret == DELETE_ERROR)		Msg::DisplayWarnMsg(Lang::get("DELETE_ERROR"));
	return ret;
}