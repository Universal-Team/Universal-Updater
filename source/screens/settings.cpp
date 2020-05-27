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

#include "credits.hpp"
#include "keyboard.hpp"
#include "settings.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);

int selectedLang;
extern bool changesMade;

Settings::Settings() {
	selectedLang = 0;
}

void Settings::Draw(void) const {
	if (mode == 0) {
		DrawSubMenu();
	} else if (mode == 1) {
		DrawLanguageSelection();
	} else if (mode == 2) {
		DrawColorChanging();
	} else if (mode == 4) {
		DrawMiscSettings();
	}
}


void Settings::DrawSubMenu(void) const {
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	}
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);
	GFX::DrawArrow(318, 240, 180.0, 1);

	GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, Lang::get("LANGUAGE"));
	GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, Lang::get("COLORS"));
	GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, Lang::get("CREDITS"));
	// Selector.
	Animation::Button(mainButtons[Selection].x, mainButtons[Selection].y, .060);
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
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, Lang::get("SELECT_LANG"), 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("SELECT_LANG"), 400);
	}
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();

	for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)languages.size();i++) {
		Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::UnselectedColor);
		line1 = languages[screenPos + i];
		if (screenPos + i == selectedLang) {
			Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, Config::SelectedColor);
		}
		Gui::DrawStringCentered(0, 50+(i*57), 0.7f, WHITE, line1, 320);
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
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	}

	if (!dropDownMenu) {
		if (colorMode == 3) {
			Gui::Draw_Rect(0, 40, 400, 45, Config::SelectedColor);
			Gui::DrawStringCentered(0, 45, 0.7f, Config::TxtColor, Lang::get("TEXT_COLOR"), 320);
		} else if (colorMode == 4) {
			Gui::Draw_Rect(0, 40, 400, 45, Config::SelectedColor);
			Gui::DrawStringCentered(0, 45, 0.7f, Config::TxtColor, Lang::get("SELECTED_COLOR"), 320);
		} else if (colorMode == 5) {
			Gui::Draw_Rect(0, 40, 400, 45, Config::UnselectedColor);
			Gui::DrawStringCentered(0, 45, 0.7f, Config::TxtColor, Lang::get("UNSELECTED_COLOR"), 320);
		} else if (colorMode == 6) {
			Gui::Draw_Rect(31, 121, (int)(((float)100/(float)100) * 338.0f), 28, Config::progressbarColor);
		} else if (colorMode == 7) {
			Gui::Draw_Rect(31, 121, (int)(((float)100/(float)100) * 338.0f), 28, Config::notFound);
		} else if (colorMode == 8) {
			Gui::Draw_Rect(31, 121, (int)(((float)100/(float)100) * 338.0f), 28, Config::outdated);
		} else if (colorMode == 9) {
			Gui::Draw_Rect(31, 121, (int)(((float)100/(float)100) * 338.0f), 28, Config::uptodate);
		} else if (colorMode == 10) {
			Gui::Draw_Rect(31, 121, (int)(((float)100/(float)100) * 338.0f), 28, Config::future);
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
		for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)colorList.size();i++) {
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::UnselectedColor);
			line1 = Lang::get(colorList[screenPos + i]);
			if(screenPos + i == colorSelection) {
				Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, Config::SelectedColor);
			}
			Gui::DrawStringCentered(0, 50+(i*57), 0.7f, Config::TxtColor, line1, 320);
		}
	} else {
		if (colorMode == 0) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(Config::Color1, 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(Config::Color1, 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(Config::Color1, 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 1) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(Config::Color2, 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(Config::Color2, 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(Config::Color2, 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 2) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(Config::Color3, 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(Config::Color3, 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(Config::Color3, 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 3) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(Config::TxtColor, 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(Config::TxtColor, 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(Config::TxtColor, 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 4) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(Config::SelectedColor, 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(Config::SelectedColor, 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(Config::SelectedColor, 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 5) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(Config::UnselectedColor, 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(Config::UnselectedColor, 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(Config::UnselectedColor, 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 6) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(Config::progressbarColor, 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(Config::progressbarColor, 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(Config::progressbarColor, 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 7) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(Config::notFound, 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(Config::notFound, 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(Config::notFound, 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 8) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(Config::outdated, 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(Config::outdated, 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(Config::outdated, 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 9) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(Config::uptodate, 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(Config::uptodate, 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(Config::uptodate, 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 10) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(Config::future, 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(Config::future, 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(Config::future, 0).c_str(), C2D_Color32(0, 0, 255, 255));
		} else if (colorMode == 11) {
			GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, ColorHelper::getColorName(Config::Button, 2).c_str(), C2D_Color32(255, 0, 0, 255));
			GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, ColorHelper::getColorName(Config::Button, 1).c_str(), C2D_Color32(0, 255, 0, 255));
			GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, ColorHelper::getColorName(Config::Button, 0).c_str(), C2D_Color32(0, 0, 255, 255));
		}
	}
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}

void Settings::DrawMiscSettings(void) const {
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	}
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	GFX::DrawButton(mainButtons2[0].x, mainButtons2[0].y, Lang::get("CHANGE_MUSICFILE"));
	GFX::DrawButton(mainButtons2[1].x, mainButtons2[1].y, Lang::get("CHANGE_BAR_STYLE"));
	GFX::DrawButton(mainButtons2[2].x, mainButtons2[2].y, Lang::get("CHANGE_KEY_DELAY"));
	GFX::DrawButton(mainButtons2[3].x, mainButtons2[3].y, Lang::get("TOGGLE_FADE"));

	// Selector.
	Animation::Button(mainButtons2[Selection].x, mainButtons2[Selection].y, .060);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}

void Settings::MiscSettingsLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_A) {
		if (Selection == 0) {
			std::string tempMusic = selectFilePath(Lang::get("SELECT_MUSIC_FILE"), "sdmc:/", {"wav"}, 2);
			if (tempMusic != "") {
				Config::MusicPath = tempMusic;
				changesMade = true;
			}
		} else if (Selection == 1) {
			if (Config::UseBars == true)	Config::UseBars = false;
			else if (Config::UseBars == false)	Config::UseBars = true;
			changesMade = true;
		} else if (Selection == 2) {
			Config::keyDelay = Input::getUint(255, Lang::get("ENTER_KEY_DELAY"));
			changesMade = true;
		} else if (Selection == 3) {
			/*if (Config::fading) {
				if (Msg::promptMsg(Lang::get("TOGGLE_FADE_DISABLE"))) {
					Config::fading = false;
					Msg::DisplayWarnMsg(Lang::get("DISABLED"));
					changesMade = true;
				}
			} else {
				if (Msg::promptMsg(Lang::get("TOGGLE_FADE_ENABLE"))) {
					Config::fading = true;
					Msg::DisplayWarnMsg(Lang::get("ENABLED"));
					changesMade = true;
				}
			}*/
		}
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons2[0])) {
			std::string tempMusic = selectFilePath(Lang::get("SELECT_MUSIC_FILE"), "sdmc:/", {"wav"}, 2);
			if (tempMusic != "") {
				Config::MusicPath = tempMusic;
				changesMade = true;
			}
		} else if (touching(touch, mainButtons2[1])) {
			if (Config::UseBars == true)	Config::UseBars = false;
			else if (Config::UseBars == false)	Config::UseBars = true;
			changesMade = true;
		} else if (touching(touch, mainButtons2[2])) {
			Config::keyDelay = Input::getUint(255, Lang::get("ENTER_KEY_DELAY"));
			changesMade = true;
		} /*else if (touching(touch, mainButtons2[3])) {
			if (Config::fading) {
				if (Msg::promptMsg(Lang::get("TOGGLE_FADE_DISABLE"))) {
					Config::fading = false;
					Msg::DisplayWarnMsg(Lang::get("DISABLED"));
					changesMade = true;
				}
			} else {
				if (Msg::promptMsg(Lang::get("TOGGLE_FADE_ENABLE"))) {
					Config::fading = true;
					Msg::DisplayWarnMsg(Lang::get("ENABLED"));
					changesMade = true;
				}
			}
		}*/
	}

	if ((hDown & KEY_B || hDown & KEY_L) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		Selection = 0;
		mode = 0;
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
}


void Settings::SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_UP) {
		if(Selection > 0)	Selection--;
	}

	if (hDown & KEY_DOWN) {
		if(Selection < 2)	Selection++;
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
				Gui::setScreen(std::make_unique<Credits>(), Config::fading, true);
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
			Gui::setScreen(std::make_unique<Credits>(), Config::fading, true);
		}
	}

	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		Gui::screenBack(Config::fading);
		return;
	}

	if ((hDown & KEY_R) || (hDown & KEY_TOUCH && touching(touch, arrowPos[4]))) {
		Selection = 0;
		mode = 4;
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
		
		keyRepeatDelay = Config::keyDelay;
	}

	if ((hHeld & KEY_UP && !keyRepeatDelay)) {
		if (selectedLang > 0) {
			selectedLang--;
		} else {
			selectedLang = (int)languages.size()-1;
		}
			
		keyRepeatDelay = Config::keyDelay;
	}

	if (hDown & KEY_A) {
		Config::lang = langsTemp[selectedLang];
		Lang::load(Config::lang);
		changesMade = true;
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

			keyRepeatDelay = Config::keyDelay;
		}

		if ((hHeld & KEY_UP && !keyRepeatDelay)) {
			if (colorSelection > 0) {
				colorSelection--;
			} else {
				colorSelection = (int)colorList.size()-1;
			}

			keyRepeatDelay = Config::keyDelay;
		}

	} else {

		if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[3]))) {
			colorSelection = colorMode;
			dropDownMenu = true;
		}

		if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
			mode = 0;
		}

		if ((hDown & KEY_L || hDown & KEY_LEFT)) {
			if(colorMode > 0)	colorMode--;
		}

		if ((hDown & KEY_R || hDown & KEY_RIGHT)) {
			if(colorMode < 11)	colorMode++;
		}

		if (hDown & KEY_TOUCH) {
			if (touching(touch, mainButtons[0])) {
				int temp = Input::getUint(255, Lang::get("ENTER_RED_RGB"));
				if(temp != -1) {
					red = temp;
					if (colorMode == 0) {
						Config::Color1 = RGBA8(red, ColorHelper::getColorValue(Config::Color1, 1), ColorHelper::getColorValue(Config::Color1, 0), 255);
					} else if (colorMode == 1) {
						Config::Color2 = RGBA8(red, ColorHelper::getColorValue(Config::Color2, 1), ColorHelper::getColorValue(Config::Color2, 0), 255);
					} else if (colorMode == 2) {
						Config::Color3 = RGBA8(red, ColorHelper::getColorValue(Config::Color3, 1), ColorHelper::getColorValue(Config::Color3, 0), 255);
					} else if (colorMode == 3) {
						Config::TxtColor = RGBA8(red, ColorHelper::getColorValue(Config::TxtColor, 1), ColorHelper::getColorValue(Config::TxtColor, 0), 255);
					} else if (colorMode == 4) {
						Config::SelectedColor = RGBA8(red, ColorHelper::getColorValue(Config::SelectedColor, 1), ColorHelper::getColorValue(Config::SelectedColor, 0), 255);
					} else if (colorMode == 5) {
						Config::UnselectedColor = RGBA8(red, ColorHelper::getColorValue(Config::UnselectedColor, 1), ColorHelper::getColorValue(Config::UnselectedColor, 0), 255);
					} else if (colorMode == 6) {
						Config::progressbarColor = RGBA8(red, ColorHelper::getColorValue(Config::progressbarColor, 1), ColorHelper::getColorValue(Config::progressbarColor, 0), 255);
					} else if (colorMode == 7) {
						Config::notFound = RGBA8(red, ColorHelper::getColorValue(Config::notFound, 1), ColorHelper::getColorValue(Config::notFound, 0), 255);
					} else if (colorMode == 8) {
						Config::outdated = RGBA8(red, ColorHelper::getColorValue(Config::outdated, 1), ColorHelper::getColorValue(Config::outdated, 0), 255);
					} else if (colorMode == 9) {
						Config::uptodate = RGBA8(red, ColorHelper::getColorValue(Config::uptodate, 1), ColorHelper::getColorValue(Config::uptodate, 0), 255);
					} else if (colorMode == 10) {
						Config::future = RGBA8(red, ColorHelper::getColorValue(Config::future, 1), ColorHelper::getColorValue(Config::future, 0), 255);
					} else if (colorMode == 11) {
						Config::Button = RGBA8(red, ColorHelper::getColorValue(Config::Button, 1), ColorHelper::getColorValue(Config::Button, 0), 255);
					}
					changesMade = true;
				}
			} else if (touching(touch, mainButtons[1])) {
				int temp = Input::getUint(255, Lang::get("ENTER_GREEN_RGB"));
				if(temp != -1) {
					green = temp;
					if (colorMode == 0) {
						Config::Color1 = RGBA8(ColorHelper::getColorValue(Config::Color1, 2), green, ColorHelper::getColorValue(Config::Color1, 0), 255);
					} else if (colorMode == 1) {
						Config::Color2 = RGBA8(ColorHelper::getColorValue(Config::Color2, 2), green, ColorHelper::getColorValue(Config::Color2, 0), 255);
					} else if (colorMode == 2) {
						Config::Color3 = RGBA8(ColorHelper::getColorValue(Config::Color3, 2), green, ColorHelper::getColorValue(Config::Color3, 0), 255);
					} else if (colorMode == 3) {
						Config::TxtColor = RGBA8(ColorHelper::getColorValue(Config::TxtColor, 2), green, ColorHelper::getColorValue(Config::TxtColor, 0), 255);
					} else if (colorMode == 4) {
						Config::SelectedColor = RGBA8(ColorHelper::getColorValue(Config::SelectedColor, 2), green, ColorHelper::getColorValue(Config::SelectedColor, 0), 255);
					} else if (colorMode == 5) {
						Config::UnselectedColor = RGBA8(ColorHelper::getColorValue(Config::UnselectedColor, 2), green, ColorHelper::getColorValue(Config::UnselectedColor, 0), 255);
					} else if (colorMode == 6) {
						Config::progressbarColor = RGBA8(ColorHelper::getColorValue(Config::progressbarColor, 2), green, ColorHelper::getColorValue(Config::progressbarColor, 0), 255);
					} else if (colorMode == 7) {
						Config::notFound = RGBA8(ColorHelper::getColorValue(Config::notFound, 2), green, ColorHelper::getColorValue(Config::notFound, 0), 255);
					} else if (colorMode == 8) {
						Config::outdated = RGBA8(ColorHelper::getColorValue(Config::outdated, 2), green, ColorHelper::getColorValue(Config::outdated, 0), 255);
					} else if (colorMode == 9) {
						Config::uptodate = RGBA8(ColorHelper::getColorValue(Config::uptodate, 2), green, ColorHelper::getColorValue(Config::uptodate, 0), 255);
					} else if (colorMode == 10) {
						Config::future = RGBA8(ColorHelper::getColorValue(Config::future, 2), green, ColorHelper::getColorValue(Config::future, 0), 255);
					} else if (colorMode == 11) {
						Config::Button = RGBA8(ColorHelper::getColorValue(Config::Button, 2), green, ColorHelper::getColorValue(Config::Button, 0), 255);
					}
					changesMade = true;
				}
			} else if (touching(touch, mainButtons[2])) {
				int temp = Input::getUint(255, Lang::get("ENTER_BLUE_RGB"));
				if(temp != -1) {
					blue = temp;
					if (colorMode == 0) {
						Config::Color1 = RGBA8(ColorHelper::getColorValue(Config::Color1, 2), ColorHelper::getColorValue(Config::Color1, 1), blue, 255);
					} else if (colorMode == 1) {
						Config::Color2 = RGBA8(ColorHelper::getColorValue(Config::Color2, 2), ColorHelper::getColorValue(Config::Color2, 1), blue, 255);
					} else if (colorMode == 2) {
						Config::Color3 = RGBA8(ColorHelper::getColorValue(Config::Color3, 2), ColorHelper::getColorValue(Config::Color3, 1), blue, 255);
					} else if (colorMode == 3) {
						Config::TxtColor = RGBA8(ColorHelper::getColorValue(Config::TxtColor, 2), ColorHelper::getColorValue(Config::TxtColor, 1), blue, 255);
					} else if (colorMode == 4) {
						Config::SelectedColor = RGBA8(ColorHelper::getColorValue(Config::SelectedColor, 2), ColorHelper::getColorValue(Config::SelectedColor, 1), blue, 255);
					} else if (colorMode == 5) {
						Config::UnselectedColor = RGBA8(ColorHelper::getColorValue(Config::UnselectedColor, 2), ColorHelper::getColorValue(Config::UnselectedColor, 1), blue, 255);
					} else if (colorMode == 6) {
						Config::progressbarColor = RGBA8(ColorHelper::getColorValue(Config::progressbarColor, 2), ColorHelper::getColorValue(Config::progressbarColor, 1), blue, 255);
					} else if (colorMode == 7) {
						Config::notFound = RGBA8(ColorHelper::getColorValue(Config::notFound, 2), ColorHelper::getColorValue(Config::notFound, 1), blue, 255);
					} else if (colorMode == 8) {
						Config::outdated = RGBA8(ColorHelper::getColorValue(Config::outdated, 2), ColorHelper::getColorValue(Config::outdated, 1), blue, 255);
					} else if (colorMode == 9) {
						Config::uptodate = RGBA8(ColorHelper::getColorValue(Config::uptodate, 2), ColorHelper::getColorValue(Config::uptodate, 1), blue, 255);
					} else if (colorMode == 10) {
						Config::future = RGBA8(ColorHelper::getColorValue(Config::future, 2), ColorHelper::getColorValue(Config::future, 1), blue, 255);
					} else if (colorMode == 11) {
						Config::Button = RGBA8(ColorHelper::getColorValue(Config::Button, 2), ColorHelper::getColorValue(Config::Button, 1), blue, 255);
					}
					changesMade = true;
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
	} else if (mode == 4) {
		MiscSettingsLogic(hDown, hHeld, touch);
	}
}