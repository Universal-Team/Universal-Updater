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
	selectedLang = Config::lang;
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
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);
	GFX::DrawArrow(318, 240, 180.0, 1);

	for (int i = 0; i < 3; i++) {
		Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, Config::UnselectedColor);
		if (Selection == i) {
			Gui::drawAnimatedSelector(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, .060, TRANSPARENT, Config::SelectedColor);
		}
	}

	Gui::DrawStringCentered(0, mainButtons[0].y+10, 0.6f, Config::TxtColor, Lang::get("LANGUAGE"), 130);
	Gui::DrawStringCentered(0, mainButtons[1].y+10, 0.6f, Config::TxtColor, Lang::get("COLORS"), 130);
	Gui::DrawStringCentered(0, mainButtons[2].y+10, 0.6f, Config::TxtColor, Lang::get("CREDITS"), 130);
}

void Settings::DrawLanguageSelection(void) const {
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, Lang::get("SELECT_LANG"), 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("SELECT_LANG"), 400);
	}
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	for (int language = 0; language < 10; language++) {
		Gui::Draw_Rect(langBlocks[language].x, langBlocks[language].y, langBlocks[language].w, langBlocks[language].h, Config::UnselectedColor);
		if (Config::lang == language) {
			Gui::drawAnimatedSelector(langBlocks[language].x, langBlocks[language].y, langBlocks[language].w, langBlocks[language].h, .060, TRANSPARENT, Config::SelectedColor);
		}
	}

	Gui::DrawString(langBlocks[0].x+25, langBlocks[0].y, 0.7f, Config::TxtColor, "Bruh", 320);
	Gui::DrawString(langBlocks[1].x+25, langBlocks[1].y, 0.7f, Config::TxtColor, "Deutsch", 320);
	Gui::DrawString(langBlocks[2].x+25, langBlocks[2].y, 0.7f, Config::TxtColor, "English", 320);
	Gui::DrawString(langBlocks[3].x+25, langBlocks[3].y, 0.7f, Config::TxtColor, "Español", 320);
	Gui::DrawString(langBlocks[4].x+25, langBlocks[4].y, 0.7f, Config::TxtColor, "Français", 320);

	Gui::DrawString(langBlocks[5].x+25, langBlocks[5].y, 0.7f, Config::TxtColor, "Italiano", 320);
	Gui::DrawString(langBlocks[6].x+25, langBlocks[6].y, 0.7f, Config::TxtColor, "Lietuvių", 320);
	Gui::DrawString(langBlocks[7].x+25, langBlocks[7].y, 0.7f, Config::TxtColor, "Português", 320);
	Gui::DrawString(langBlocks[8].x+25, langBlocks[8].y, 0.7f, Config::TxtColor, "Русский", 320);
	Gui::DrawString(langBlocks[9].x+25, langBlocks[9].y, 0.7f, Config::TxtColor, "日本語", 320);
}

void Settings::DrawColorChanging(void) const {
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	}

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
	}



	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	GFX::DrawArrow(0, 0, 0, 1);
	GFX::DrawArrow(318, 22, 180.0, 1);

	for (int i = 0; i < 11; i++) {
		if (colorMode == i) {
			Gui::drawAnimatedSelector(24 + i * 25, 2, 16, 16, .060, TRANSPARENT, C2D_Color32(140, 140, 140, 255));
		}
	}

	for (int i = 0; i < 11; i++) {
		Gui::DrawString(28 + i * 25, 2, 0.5f, WHITE, std::to_string(i+1), 400);
	}

	Gui::Draw_Rect(buttons[0].x, buttons[0].y, 95, 41, C2D_Color32(255, 0, 0, 255));
	Gui::Draw_Rect(buttons[1].x, buttons[1].y, 95, 41, C2D_Color32(0, 255, 0, 255));
	Gui::Draw_Rect(buttons[2].x, buttons[2].y, 95, 41, C2D_Color32(0, 0, 255, 255));

	if (colorMode == 0) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("BAR_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color1, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color1, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color1, 0).c_str(), 400);
	} else if (colorMode == 1) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("TOP_BG_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color2, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color2, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color2, 0).c_str(), 400);
	} else if (colorMode == 2) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("BOTTOM_BG_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color3, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color3, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::Color3, 0).c_str(), 400);
	} else if (colorMode == 3) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("TEXT_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::TxtColor, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::TxtColor, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::TxtColor, 0).c_str(), 400);
	} else if (colorMode == 4) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("SELECTED_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::SelectedColor, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::SelectedColor, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::SelectedColor, 0).c_str(), 400);
	} else if (colorMode == 5) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("UNSELECTED_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::UnselectedColor, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::UnselectedColor, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::UnselectedColor, 0).c_str(), 400);
	} else if (colorMode == 6) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("PROGRESSBAR_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::progressbarColor, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::progressbarColor, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::progressbarColor, 0).c_str(), 400);
	} else if (colorMode == 7) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("NOT_FOUND_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::notFound, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::notFound, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::notFound, 0).c_str(), 400);
	} else if (colorMode == 8) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("OUTDATED_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::outdated, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::outdated, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::outdated, 0).c_str(), 400);
	} else if (colorMode == 9) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("UPTODATE_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::uptodate, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::uptodate, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::uptodate, 0).c_str(), 400);
	} else if (colorMode == 10) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("FUTURE_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::future, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::future, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, WHITE, ColorHelper::getColorName(Config::future, 0).c_str(), 400);
	}
}

void Settings::DrawMiscSettings(void) const {
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	}

	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	for (int i = 0; i < 3; i++) {
		Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, Config::UnselectedColor);
		if (Selection == i) {
			Gui::drawAnimatedSelector(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, .060, TRANSPARENT, Config::SelectedColor);
		}
	}

	Gui::DrawStringCentered(0, mainButtons[0].y+10, 0.6f, Config::TxtColor, Lang::get("CHANGE_MUSICFILE"), 130);
	Gui::DrawStringCentered(0, mainButtons[1].y+10, 0.6f, Config::TxtColor, Lang::get("CHANGE_BAR_STYLE"), 130);
	Gui::DrawStringCentered(0, mainButtons[2].y+10, 0.6f, Config::TxtColor, Lang::get("ENABLE_GODMODE"), 130);
}

void Settings::MiscSettingsLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_UP) {
		if (Selection > 0)	Selection--;
	}

	if (hDown & KEY_DOWN) {
		if (Selection < 2)	Selection++;
	}

	if (hDown & KEY_A) {
		if (Selection == 0) {
			std::string tempMusic = selectFilePath(Lang::get("SELECT_MUSIC_FILE"), {"wav"}, 2);
			if (tempMusic != "") {
				Config::MusicPath = tempMusic;
				changesMade = true;
			}
		} else if (Selection == 1) {
			if (Config::UseBars == true)	Config::UseBars = false;
			else if (Config::UseBars == false)	Config::UseBars = true;
			changesMade = true;
		} else if (Selection == 2) {
			if (Msg::promptMsg(Lang::get("ENABLE_GODMODE_PROMPT"))) {
				Config::GodMode = true;
			}
		}
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			std::string tempMusic = selectFilePath(Lang::get("SELECT_MUSIC_FILE"), {"wav"}, 2);
			if (tempMusic != "") {
				Config::MusicPath = tempMusic;
				changesMade = true;
			}
		} else if (touching(touch, mainButtons[1])) {
			if (Config::UseBars == true)	Config::UseBars = false;
			else if (Config::UseBars == false)	Config::UseBars = true;
			changesMade = true;
		} else if (touching(touch, mainButtons[2])) {
			if (Msg::promptMsg(Lang::get("ENABLE_GODMODE_PROMPT"))) {
				Config::GodMode = true;
			}
		}
	}

	if ((hDown & KEY_B || hDown & KEY_L) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		Selection = 0;
		mode = 0;
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
		if (Selection + 1 == 3)	Gui::setScreen(std::make_unique<Credits>());
		else	mode = Selection+1;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			mode = 1;
		} else if (touching(touch, mainButtons[1])) {
			mode = 2;
		} else if (touching(touch, mainButtons[2])) {
			Gui::setScreen(std::make_unique<Credits>());
		}
	}

	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		Gui::screenBack();
		return;
	}

	if ((hDown & KEY_R) || (hDown & KEY_TOUCH && touching(touch, arrowPos[4]))) {
		Selection = 0;
		mode = 4;
	}
}

void Settings::LanguageSelection(u32 hDown, touchPosition touch) {

	if (hDown & KEY_TOUCH) {
		for (int language = 0; language < 10; language++) {
			if (touching(touch, langBlocks[language])) {
				selectedLang = language;
				Config::lang = language;
				Lang::load(Config::lang);
				changesMade = true;
			}
		}
	}

	if (hDown & KEY_UP) {
		if(selectedLang > 0) {
			selectedLang--;
			Config::lang = selectedLang;
			Lang::load(Config::lang);
			changesMade = true;
		}
	}

	if (hDown & KEY_DOWN) {
		if(selectedLang < 9) {
			selectedLang++;
			Config::lang = selectedLang;
			Lang::load(Config::lang);
			changesMade = true;
		}
	}


	if (hDown & KEY_LEFT) {
		if (selectedLang > 4) {
			selectedLang -= 5;
			Config::lang = selectedLang;
			Lang::load(Config::lang);
			changesMade = true;
		}
	}

	if (hDown & KEY_RIGHT) {
		if (selectedLang < 5) {
			selectedLang += 5;
			Config::lang = selectedLang;
			Lang::load(Config::lang);
			changesMade = true;
		}
	}

	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		mode = 0;
	}
}


void Settings::colorChanging(u32 hDown, touchPosition touch) {
	int red;
	int green;
	int blue;

	if (hDown & KEY_TOUCH) {
		for (int i = 0; i < 11; i++) {
			if(touch.px > 24 + i * 25 && touch.px < 24 + i * 25+16 && touch.py > 2 && touch.py < 2+16) {
				colorMode = i;
			}
		}
	}


	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		mode = 0;
	}

	if ((hDown & KEY_L || hDown & KEY_LEFT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[0]))) {
		if(colorMode > 0)	colorMode--;
	}

	if ((hDown & KEY_R || hDown & KEY_RIGHT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[1]))) {
		if(colorMode < 10)	colorMode++;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, buttons[0])) {
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
				}
				changesMade = true;
			}


		} else if (touching(touch, buttons[1])) {
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
				}
				changesMade = true;
			}
		} else if (touching(touch, buttons[2])) {
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
				}
				changesMade = true;
			}
		}
	}
}

void Settings::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (mode == 0) {
		SubMenuLogic(hDown, hHeld, touch);
	} else if (mode == 1) {
		LanguageSelection(hDown, touch);
	} else if (mode == 2) {
		colorChanging(hDown, touch);
	} else if (mode == 4) {
		MiscSettingsLogic(hDown, hHeld, touch);
	}
}