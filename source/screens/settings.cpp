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

#include "keyboard.hpp"
#include "overlay.hpp"
#include "settings.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern std::unique_ptr<Config> config;

int selectedLang;
extern bool changesMade;

Settings::Settings() { selectedLang = 0; }

void Settings::Draw(void) const {
	if (mode == 0) {
		DrawSubMenu();
	} else if (mode == 1) {
		DrawLanguageSelection();
	} else if (mode == 2) {
		DrawColorChanging();
	}
}


void Settings::DrawSubMenu(void) const {
	GFX::DrawTop();
	Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.7f, config->textColor(), "Universal-Updater", 400);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);
	GFX::DrawArrow(318, 240, 180.0, 1);

	if (this->settingPage == 0) {
		GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, Lang::get("LANGUAGE"));
		GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, Lang::get("COLORS"));
		GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, Lang::get("CHANGE_BAR_STYLE"));
	} else if (this->settingPage == 1) {
		GFX::DrawButton(mainButtons2[0].x, mainButtons2[0].y, Lang::get("CHANGE_MUSICFILE"));
		GFX::DrawButton(mainButtons2[1].x, mainButtons2[1].y, Lang::get("CHANGE_KEY_DELAY"));
		GFX::DrawButton(mainButtons2[2].x, mainButtons2[2].y, Lang::get("TOGGLE_FADE"));
		GFX::DrawButton(mainButtons2[3].x, mainButtons2[3].y, Lang::get("TOGGLE_PROGRESSBAR"));
	} else if (this->settingPage == 2) {
		GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, Lang::get("CHANGE_3DSX_PATH"));
		GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, Lang::get("CHANGE_NDS_PATH"));
		GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, Lang::get("CHANGE_ARCHIVE_PATH"));
	}

	// Selector.
	if (this->settingPage == 0 || this->settingPage == 2) {
		Animation::Button(mainButtons[Selection].x, mainButtons[Selection].y, .060);
	} else {
		Animation::Button(mainButtons2[Selection].x, mainButtons2[Selection].y, .060);
	}
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}

const std::vector<std::string> languages = {
	"Bruh",
	"Dansk",
	"Deutsch",
	"English",
	"Español",
	"Français",
	"Italiano",
	"Lietuvių",
	"Polski",
	"Português",
	"Русский",
	"日本語"
};

void Settings::DrawLanguageSelection(void) const {
	std::string line1;
	GFX::DrawTop();
	Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.7f, config->textColor(), Lang::get("SELECT_LANG"), 400);

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();

	for(int i = 0; i < ENTRIES_PER_SCREEN && i < (int)languages.size(); i++) {
		Gui::Draw_Rect(0, 40+(i*57), 320, 45, config->unselectedColor());
		line1 = languages[screenPos + i];
		if (screenPos + i == selectedLang) {
			Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, config->selectedColor());
		}

		Gui::DrawStringCentered(0, 50+(i*57), 0.7f, config->textColor(), line1, 320);
	}

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}

const std::vector<std::string> colorList = {
	"BAR_COLOR",
	"TOP_BG_COLOR",
	"BOTTOM_BG_COLOR",
	"TEXT_COLOR",
	"SELECTED_COLOR",
	"UNSELECTED_COLOR",
	"PROGRESSBAR_COLOR",
	"NOT_FOUND_COLOR",
	"OUTDATED_COLOR",
	"UPTODATE_COLOR",
	"FUTURE_COLOR",
	"BUTTON_COLOR"
};


void Settings::DrawColorChanging(void) const {
	std::string line1;
	GFX::DrawTop();
	Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.7f, config->textColor(), "Universal-Updater", 400);

	if (!dropDownMenu) {
		if (colorMode == 3) {
			Gui::Draw_Rect(0, 40, 400, 45, config->selectedColor());
			Gui::DrawStringCentered(0, 45, 0.7f, config->textColor(), Lang::get("TEXT_COLOR"), 320);
		} else if (colorMode == 4) {
			Gui::Draw_Rect(0, 40, 400, 45, config->selectedColor());
			Gui::DrawStringCentered(0, 45, 0.7f, config->textColor(), Lang::get("SELECTED_COLOR"), 320);
		} else if (colorMode == 5) {
			Gui::Draw_Rect(0, 40, 400, 45, config->unselectedColor());
			Gui::DrawStringCentered(0, 45, 0.7f, config->textColor(), Lang::get("UNSELECTED_COLOR"), 320);
		} else if (colorMode == 6) {
			Gui::Draw_Rect(31, 121, (int)(((float)100/(float)100) * 338.0f), 28, config->progressbarColor());
		} else if (colorMode == 7) {
			Gui::Draw_Rect(31, 121, (int)(((float)100/(float)100) * 338.0f), 28, config->notfoundColor());
		} else if (colorMode == 8) {
			Gui::Draw_Rect(31, 121, (int)(((float)100/(float)100) * 338.0f), 28, config->outdatedColor());
		} else if (colorMode == 9) {
			Gui::Draw_Rect(31, 121, (int)(((float)100/(float)100) * 338.0f), 28, config->uptodateColor());
		} else if (colorMode == 10) {
			Gui::Draw_Rect(31, 121, (int)(((float)100/(float)100) * 338.0f), 28, config->futureColor());
		} else if (colorMode == 11) {
			GFX::DrawButton(100, 100, "");
		}

		Gui::DrawStringCentered(0, 215, 0.7f, WHITE, Lang::get(colorList[colorMode]), 320);
	}


	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();
	if (!dropDownMenu) {
		GFX::DrawArrow(0, 218, 0, 1);
		GFX::DrawSpriteBlend(sprites_dropdown_idx, arrowPos[5].x, arrowPos[5].y);
	}

	if (dropDownMenu) {
		for(int i = 0; i < ENTRIES_PER_SCREEN && i < (int)colorList.size(); i++) {
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, config->unselectedColor());
			line1 = Lang::get(colorList[screenPos + i]);
			if (screenPos + i == colorSelection) {
				Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, config->selectedColor());
			}

			Gui::DrawStringCentered(0, 50+(i*57), 0.7f, config->textColor(), line1, 320);
		}
	} else {
		if (colorMode == 0) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(config->barColor(), 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(config->barColor(), 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(config->barColor(), 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 1) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(config->topBG(), 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(config->topBG(), 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(config->topBG(), 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 2) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(config->bottomBG(), 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(config->bottomBG(), 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(config->bottomBG(), 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 3) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(config->textColor(), 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(config->textColor(), 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(config->textColor(), 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 4) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(config->selectedColor(), 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(config->selectedColor(), 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(config->selectedColor(), 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 5) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(config->unselectedColor(), 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(config->unselectedColor(), 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(config->unselectedColor(), 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 6) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(config->progressbarColor(), 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(config->progressbarColor(), 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(config->progressbarColor(), 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 7) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(config->notfoundColor(), 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(config->notfoundColor(), 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(config->notfoundColor(), 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 8) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(config->outdatedColor(), 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(config->outdatedColor(), 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(config->outdatedColor(), 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 9) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(config->uptodateColor(), 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(config->uptodateColor(), 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(config->uptodateColor(), 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 10) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(config->futureColor(), 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(config->futureColor(), 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(config->futureColor(), 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 11) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(config->buttonColor(), 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(config->buttonColor(), 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(config->buttonColor(), 0).c_str(), C2D_Color32(0, 0, 255, 255));
		}
	}

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}


void Settings::SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if ((hDown & KEY_B)) {
		Gui::screenBack(config->screenFade());
		return;
	}

	if ((hDown & KEY_R) || (hDown & KEY_TOUCH && touching(touch, arrowPos[4]))) {
		if (this->settingPage < 2) {
			this->settingPage++;
			Selection = 0;
		}
	}

	if ((hDown & KEY_L) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		if (this->settingPage > 0) {
			this->settingPage--;
			Selection = 0;
		}
	}

	if (this->settingPage == 0) {
		if (hDown & KEY_UP) {
			if (Selection > 0)	Selection--;
		}

		if (hDown & KEY_DOWN) {
			if (Selection < 2)	Selection++;
		}

		if (hDown & KEY_A) {
			switch (Selection) {
				case 0:
					screenPos = 0;
					selectedLang = 0;
					mode = 1;
					break;
				case 1:
					screenPos = 0;
					mode = 2;
					break;
				case 2:
					if (config->useBars())	config->useBars(false);
					else	config->useBars(true);
					break;
			}
		}

		if (hDown & KEY_TOUCH) {
			if (touching(touch, mainButtons[0])) {
				screenPos = 0;
				selectedLang = 0;
				mode = 1;
			} else if (touching(touch, mainButtons[1])) {
				screenPos = 0;
				mode = 2;
			} else if (touching(touch, mainButtons[2])) {
				if (config->useBars())	config->useBars(false);
				else	config->useBars(true);
			}
		}
	} else if (this->settingPage == 1) {
		if (hDown & KEY_A) {
			if (Selection == 0) {
				std::string tempMusic = selectFilePath(Lang::get("SELECT_MUSIC_FILE"), "sdmc:/", {"wav"}, 2);
				if (tempMusic != "") {
					config->musicPath(tempMusic);
				}
			} else if (Selection == 1) {
				config->keyDelay(Input::getUint(255, Lang::get("ENTER_KEY_DELAY")));
			} else if (Selection == 2) {
				if (config->screenFade()) {
					if (Msg::promptMsg(Lang::get("TOGGLE_FADE_DISABLE"))) {
						config->screenFade(false);
						Msg::DisplayWarnMsg(Lang::get("DISABLED"));
					}
				} else {
					if (Msg::promptMsg(Lang::get("TOGGLE_FADE_ENABLE"))) {
						config->screenFade(true);
						Msg::DisplayWarnMsg(Lang::get("ENABLED"));
					}
				}
			} else if (Selection == 3) {
				if (config->progressDisplay()) {
					if (Msg::promptMsg(Lang::get("PROGRESS_BAR_DISABLE"))) {
						config->progressDisplay(false);
						Msg::DisplayWarnMsg(Lang::get("DISABLED"));
					}
				} else {
					if (Msg::promptMsg(Lang::get("PROGRESS_BAR_ENABLE"))) {
						config->progressDisplay(true);
						Msg::DisplayWarnMsg(Lang::get("ENABLED"));
					}
				}
			}
		}

		if (hDown & KEY_TOUCH) {
			if (touching(touch, mainButtons2[0])) {
				std::string tempMusic = selectFilePath(Lang::get("SELECT_MUSIC_FILE"), "sdmc:/", {"wav"}, 2);
				if (tempMusic != "") {
					config->musicPath(tempMusic);
				}
			} else if (touching(touch, mainButtons2[1])) {
				config->keyDelay(Input::getUint(255, Lang::get("ENTER_KEY_DELAY")));
			} else if (touching(touch, mainButtons2[2])) {
				if (config->screenFade()) {
					if (Msg::promptMsg(Lang::get("TOGGLE_FADE_DISABLE"))) {
						config->screenFade(false);
						Msg::DisplayWarnMsg(Lang::get("DISABLED"));
					}
				} else {
					if (Msg::promptMsg(Lang::get("TOGGLE_FADE_ENABLE"))) {
						config->screenFade(true);
						Msg::DisplayWarnMsg(Lang::get("ENABLED"));
					}
				}
			} else if (touching(touch, mainButtons2[3])) {
				if (config->progressDisplay()) {
					if (Msg::promptMsg(Lang::get("PROGRESS_BAR_DISABLE"))) {
						config->progressDisplay(false);
						Msg::DisplayWarnMsg(Lang::get("DISABLED"));
					}
				} else {
					if (Msg::promptMsg(Lang::get("PROGRESS_BAR_ENABLE"))) {
						config->progressDisplay(true);
						Msg::DisplayWarnMsg(Lang::get("ENABLED"));
					}
				}
			}
		}
		
		// Navigation.
		if (hDown & KEY_UP) {
			if (Selection > 1)	Selection -= 2;
		} else if (hDown & KEY_DOWN) {
			if (Selection < 2)	Selection += 2;
		} else if (hDown & KEY_LEFT) {
			if (Selection%2) Selection--;
		} else if (hDown & KEY_RIGHT) {
			if (!(Selection%2)) Selection++;
		}
	} else if (this->settingPage == 2) {
		if (hDown & KEY_UP) {
			if (Selection > 0)	Selection--;
		}

		if (hDown & KEY_DOWN) {
			if (Selection < 2)	Selection++;
		}

		if (hDown & KEY_A) {
			std::string tempPath;
			switch (Selection) {
				case 0:
					tempPath = selectFilePath(Lang::get("SELECT_3DSX_PATH"), config->_3dsxpath(), {});
					if (tempPath != "") config->_3dsxpath(tempPath);
					break;
				case 1:
					tempPath = selectFilePath(Lang::get("SELECT_NDS_PATH"), config->ndspath(), {});
					if (tempPath != "")	config->ndspath(tempPath);
					break;
				case 2:
					tempPath = selectFilePath(Lang::get("SELECT_ARCHIVE_PATH"), config->archivepath(), {});
					if (tempPath != "")	config->archivepath(tempPath);
					break;
			}
		}

		if (hDown & KEY_TOUCH) {
			if (touching(touch, mainButtons[0])) {
				std::string tempPath = selectFilePath(Lang::get("SELECT_3DSX_PATH"), config->_3dsxpath(), {});
				if (tempPath != "") config->_3dsxpath(tempPath);
			} else if (touching(touch, mainButtons[1])) {
				std::string tempPath = selectFilePath(Lang::get("SELECT_NDS_PATH"), config->ndspath(), {});
				if (tempPath != "")	config->ndspath(tempPath);
			} else if (touching(touch, mainButtons[2])) {
				std::string tempPath = selectFilePath(Lang::get("SELECT_ARCHIVE_PATH"), config->archivepath(), {});
				if (tempPath != "")	config->archivepath(tempPath);
			}
		}
	}
}

std::string langsTemp[] = {"br", "da", "de", "en", "es", "fr", "it", "lt", "pl", "pt", "ru", "jp"};
void Settings::LanguageSelection(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;

	if ((hHeld & KEY_DOWN && !keyRepeatDelay)) {
		if (selectedLang < (int)languages.size()-1) {
			selectedLang++;
		} else {
			selectedLang = 0;
		}
		
		keyRepeatDelay = config->keyDelay();
	}

	if ((hHeld & KEY_UP && !keyRepeatDelay)) {
		if (selectedLang > 0) {
			selectedLang--;
		} else {
			selectedLang = (int)languages.size()-1;
		}
			
		keyRepeatDelay = config->keyDelay();
	}

	if (hDown & KEY_A) {
		config->language(langsTemp[selectedLang]);
		Lang::load(config->language());
		mode = 0;
	}

	if ((hDown & KEY_B)) {
		mode = 0;
	}

	if (selectedLang < screenPos) {
		screenPos = selectedLang;
	} else if (selectedLang > screenPos + ENTRIES_PER_SCREEN - 1) {
		screenPos = selectedLang - ENTRIES_PER_SCREEN + 1;
	}
}


void Settings::colorChanging(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;
	int red;
	int green;
	int blue;

	if (dropDownMenu) {
		if (hDown & KEY_A) {
			colorMode = colorSelection;
			dropDownMenu = false;
		}

		if ((hHeld & KEY_DOWN && !keyRepeatDelay)) {
			if (colorSelection < (int)colorList.size()-1) {
				colorSelection++;
			} else {
				colorSelection = 0;
			}

			keyRepeatDelay = config->keyDelay();
		}

		if ((hHeld & KEY_UP && !keyRepeatDelay)) {
			if (colorSelection > 0) {
				colorSelection--;
			} else {
				colorSelection = (int)colorList.size()-1;
			}

			keyRepeatDelay = config->keyDelay();
		}

	} else {
		if (hDown & KEY_A) {
			switch(this->colorMode) {
				case 0:
					config->barColor(Overlays::SelectColor(config->barColor()));
					break;
				case 1:
					config->topBG(Overlays::SelectColor(config->topBG()));
					break;
				case 2:
					config->bottomBG(Overlays::SelectColor(config->bottomBG()));
					break;
				case 3:
					config->textColor(Overlays::SelectColor(config->textColor()));
					break;
				case 4:
					config->selectedColor(Overlays::SelectColor(config->selectedColor()));
					break;
				case 5:
					config->unselectedColor(Overlays::SelectColor(config->unselectedColor()));
					break;
				case 6:
					config->progressbarColor(Overlays::SelectColor(config->progressbarColor()));
					break;
				case 7:
					config->notfoundColor(Overlays::SelectColor(config->notfoundColor()));
					break;
				case 8:
					config->outdatedColor(Overlays::SelectColor(config->outdatedColor()));
					break;
				case 9:
					config->uptodateColor(Overlays::SelectColor(config->uptodateColor()));
					break;
				case 10:
					config->futureColor(Overlays::SelectColor(config->futureColor()));
					break;
				case 11:
					config->buttonColor(Overlays::SelectColor(config->buttonColor()));
					break;
			}
		}

		if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[3]))) {
			colorSelection = colorMode;
			dropDownMenu = true;
		}

		if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
			mode = 0;
		}

		if ((hDown & KEY_L || hDown & KEY_LEFT)) {
			if (colorMode > 0)	colorMode--;
		}

		if ((hDown & KEY_R || hDown & KEY_RIGHT)) {
			if (colorMode < 11)	colorMode++;
		}

		if (hDown & KEY_TOUCH) {
			if (touching(touch, mainButtons[0])) {
				int temp = Input::getUint(255, Lang::get("ENTER_RED_RGB"));
				if (temp != -1) {
					red = temp;
					if (colorMode == 0) {
						config->barColor(RGBA8(red, ColorHelper::getColorValue(config->barColor(), 1), ColorHelper::getColorValue(config->barColor(), 0), 255));
					} else if (colorMode == 1) {
						config->topBG(RGBA8(red, ColorHelper::getColorValue(config->topBG(), 1), ColorHelper::getColorValue(config->topBG(), 0), 255));
					} else if (colorMode == 2) {
						config->bottomBG(RGBA8(red, ColorHelper::getColorValue(config->bottomBG(), 1), ColorHelper::getColorValue(config->bottomBG(), 0), 255));
					} else if (colorMode == 3) {
						config->textColor(RGBA8(red, ColorHelper::getColorValue(config->textColor(), 1), ColorHelper::getColorValue(config->textColor(), 0), 255));
					} else if (colorMode == 4) {
						config->selectedColor(RGBA8(red, ColorHelper::getColorValue(config->selectedColor(), 1), ColorHelper::getColorValue(config->selectedColor(), 0), 255));
					} else if (colorMode == 5) {
						config->unselectedColor(RGBA8(red, ColorHelper::getColorValue(config->unselectedColor(), 1), ColorHelper::getColorValue(config->unselectedColor(), 0), 255));
					} else if (colorMode == 6) {
						config->progressbarColor(RGBA8(red, ColorHelper::getColorValue(config->progressbarColor(), 1), ColorHelper::getColorValue(config->progressbarColor(), 0), 255));
					} else if (colorMode == 7) {
						config->notfoundColor(RGBA8(red, ColorHelper::getColorValue(config->notfoundColor(), 1), ColorHelper::getColorValue(config->notfoundColor(), 0), 255));
					} else if (colorMode == 8) {
						config->outdatedColor(RGBA8(red, ColorHelper::getColorValue(config->outdatedColor(), 1), ColorHelper::getColorValue(config->outdatedColor(), 0), 255));
					} else if (colorMode == 9) {
						config->uptodateColor(RGBA8(red, ColorHelper::getColorValue(config->uptodateColor(), 1), ColorHelper::getColorValue(config->uptodateColor(), 0), 255));
					} else if (colorMode == 10) {
						config->futureColor(RGBA8(red, ColorHelper::getColorValue(config->futureColor(), 1), ColorHelper::getColorValue(config->futureColor(), 0), 255));
					} else if (colorMode == 11) {
						config->buttonColor(RGBA8(red, ColorHelper::getColorValue(config->buttonColor(), 1), ColorHelper::getColorValue(config->buttonColor(), 0), 255));
					}
				}
			} else if (touching(touch, mainButtons[1])) {
				int temp = Input::getUint(255, Lang::get("ENTER_GREEN_RGB"));
				if (temp != -1) {
					green = temp;
					if (colorMode == 0) {
						config->barColor(RGBA8(ColorHelper::getColorValue(config->barColor(), 2), green, ColorHelper::getColorValue(config->barColor(), 0), 255));
					} else if (colorMode == 1) {
						config->topBG(RGBA8(ColorHelper::getColorValue(config->topBG(), 2), green, ColorHelper::getColorValue(config->topBG(), 0), 255));
					} else if (colorMode == 2) {
						config->bottomBG(RGBA8(ColorHelper::getColorValue(config->bottomBG(), 2), green, ColorHelper::getColorValue(config->bottomBG(), 0), 255));
					} else if (colorMode == 3) {
						config->textColor(RGBA8(ColorHelper::getColorValue(config->textColor(), 2), green, ColorHelper::getColorValue(config->textColor(), 0), 255));
					} else if (colorMode == 4) {
						config->selectedColor(RGBA8(ColorHelper::getColorValue(config->selectedColor(), 2), green, ColorHelper::getColorValue(config->selectedColor(), 0), 255));
					} else if (colorMode == 5) {
						config->unselectedColor(RGBA8(ColorHelper::getColorValue(config->unselectedColor(), 2), green, ColorHelper::getColorValue(config->unselectedColor(), 0), 255));
					} else if (colorMode == 6) {
						config->progressbarColor(RGBA8(ColorHelper::getColorValue(config->progressbarColor(), 2), green, ColorHelper::getColorValue(config->progressbarColor(), 0), 255));
					} else if (colorMode == 7) {
						config->notfoundColor(RGBA8(ColorHelper::getColorValue(config->notfoundColor(), 2), green, ColorHelper::getColorValue(config->notfoundColor(), 0), 255));
					} else if (colorMode == 8) {
						config->outdatedColor(RGBA8(ColorHelper::getColorValue(config->outdatedColor(), 2), green, ColorHelper::getColorValue(config->outdatedColor(), 0), 255));
					} else if (colorMode == 9) {
						config->uptodateColor(RGBA8(ColorHelper::getColorValue(config->uptodateColor(), 2), green, ColorHelper::getColorValue(config->uptodateColor(), 0), 255));
					} else if (colorMode == 10) {
						config->futureColor(RGBA8(ColorHelper::getColorValue(config->futureColor(), 2), green, ColorHelper::getColorValue(config->futureColor(), 0), 255));
					} else if (colorMode == 11) {
						config->buttonColor(RGBA8(ColorHelper::getColorValue(config->buttonColor(), 2), green, ColorHelper::getColorValue(config->buttonColor(), 0), 255));
					}
				}
			} else if (touching(touch, mainButtons[2])) {
				int temp = Input::getUint(255, Lang::get("ENTER_BLUE_RGB"));
				if (temp != -1) {
					blue = temp;
					if (colorMode == 0) {
						config->barColor(RGBA8(ColorHelper::getColorValue(config->barColor(), 2), ColorHelper::getColorValue(config->barColor(), 1), blue, 255));
					} else if (colorMode == 1) {
						config->topBG(RGBA8(ColorHelper::getColorValue(config->topBG(), 2), ColorHelper::getColorValue(config->topBG(), 1), blue, 255));
					} else if (colorMode == 2) {
						config->bottomBG(RGBA8(ColorHelper::getColorValue(config->bottomBG(), 2), ColorHelper::getColorValue(config->bottomBG(), 1), blue, 255));
					} else if (colorMode == 3) {
						config->textColor(RGBA8(ColorHelper::getColorValue(config->textColor(), 2), ColorHelper::getColorValue(config->textColor(), 1), blue, 255));
					} else if (colorMode == 4) {
						config->selectedColor(RGBA8(ColorHelper::getColorValue(config->selectedColor(), 2), ColorHelper::getColorValue(config->selectedColor(), 1), blue, 255));
					} else if (colorMode == 5) {
						config->unselectedColor(RGBA8(ColorHelper::getColorValue(config->unselectedColor(), 2), ColorHelper::getColorValue(config->unselectedColor(), 1), blue, 255));
					} else if (colorMode == 6) {
						config->progressbarColor(RGBA8(ColorHelper::getColorValue(config->progressbarColor(), 2), ColorHelper::getColorValue(config->progressbarColor(), 1), blue, 255));
					} else if (colorMode == 7) {
						config->notfoundColor(RGBA8(ColorHelper::getColorValue(config->notfoundColor(), 2), ColorHelper::getColorValue(config->notfoundColor(), 1), blue, 255));
					} else if (colorMode == 8) {
						config->outdatedColor(RGBA8(ColorHelper::getColorValue(config->outdatedColor(), 2), ColorHelper::getColorValue(config->outdatedColor(), 1), blue, 255));
					} else if (colorMode == 9) {
						config->uptodateColor(RGBA8(ColorHelper::getColorValue(config->uptodateColor(), 2), ColorHelper::getColorValue(config->uptodateColor(), 1), blue, 255));
					} else if (colorMode == 10) {
						config->futureColor(RGBA8(ColorHelper::getColorValue(config->futureColor(), 2), ColorHelper::getColorValue(config->futureColor(), 1), blue, 255));
					} else if (colorMode == 11) {
						config->buttonColor(RGBA8(ColorHelper::getColorValue(config->buttonColor(), 2), ColorHelper::getColorValue(config->buttonColor(), 1), blue, 255));
					}
				}
			}
		}
	}

	if (colorSelection < screenPos) {
		screenPos = colorSelection;
	} else if (colorSelection > screenPos + ENTRIES_PER_SCREEN - 1) {
		screenPos = colorSelection - ENTRIES_PER_SCREEN + 1;
	}
}

void Settings::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (mode == 0) {
		SubMenuLogic(hDown, hHeld, touch);
	} else if (mode == 1) {
		LanguageSelection(hDown, hHeld, touch);
	} else if (mode == 2) {
		colorChanging(hDown, hHeld, touch);
	}
}