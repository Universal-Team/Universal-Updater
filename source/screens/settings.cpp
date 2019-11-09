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

#include "screens/settings.hpp"

#include "utils/config.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);

int mode;
int colorMode = 0;

void Settings::Draw(void) const {
	if (mode == 0) {
		DrawLanguageSelection();
	} else if (mode == 1) {
		DrawColorChanging();
	}
}

void Settings::DrawLanguageSelection(void) const {
	Gui::DrawTop();
	Gui::DrawString((400-Gui::GetStringWidth(0.8f, Lang::get("SELECT_LANG")))/2, 2, 0.8f, Config::TxtColor, Lang::get("SELECT_LANG"), 400);
	Gui::DrawBottom();

	for (int language = 0; language < 10; language++) {
		if (Config::lang == language) {
			Gui::Draw_Rect(langBlocks[language].x, langBlocks[language].y, langBlocks[language].w, langBlocks[language].h, Config::SelectedColor);
		} else {
			Gui::Draw_Rect(langBlocks[language].x, langBlocks[language].y, langBlocks[language].w, langBlocks[language].h, Config::UnselectedColor);
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
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Updater", 400);

	if (colorMode == 3) {
		Gui::Draw_Rect(0, 40, 400, 45, Config::SelectedColor);
		Gui::DrawStringCentered(0, 45, 0.7f, Config::TxtColor, Lang::get("TEXT_COLOR"), 320);
	} else if (colorMode == 4) {
		Gui::Draw_Rect(0, 40, 400, 45, Config::SelectedColor);
		Gui::DrawStringCentered(0, 45, 0.7f, Config::TxtColor, Lang::get("SELECTED_COLOR"), 320);
	} else if (colorMode == 5) {
		Gui::Draw_Rect(0, 40, 400, 45, Config::UnselectedColor);
		Gui::DrawStringCentered(0, 45, 0.7f, Config::TxtColor, Lang::get("UNSELECTED_COLOR"), 320);
	}



	Gui::DrawBottom();

	for (int i = 0; i < 6; i++) {
		if (colorMode == i) {
			Gui::Draw_Rect(25 + i * 25, 5, 16, 16, Config::SelectedColor);
		}
	}

	Gui::DrawString(29 + 0 * 25, 5, 0.5f, Config::TxtColor, "1", 400);
	Gui::DrawString(29 + 1 * 25, 5, 0.5f, Config::TxtColor, "2", 400);
	Gui::DrawString(29 + 2 * 25, 5, 0.5f, Config::TxtColor, "3", 400);
	Gui::DrawString(29 + 3 * 25, 5, 0.5f, Config::TxtColor, "4", 400);
	Gui::DrawString(29 + 4 * 25, 5, 0.5f, Config::TxtColor, "5", 400);
	Gui::DrawString(29 + 5 * 25, 5, 0.5f, Config::TxtColor, "6", 400);

	Gui::Draw_Rect(buttons[0].x, buttons[0].y, 95, 41, C2D_Color32(255, 0, 0, 255));
	Gui::Draw_Rect(buttons[1].x, buttons[1].y, 95, 41, C2D_Color32(0, 255, 0, 255));
	Gui::Draw_Rect(buttons[2].x, buttons[2].y, 95, 41, C2D_Color32(0, 0, 255, 255));

	if (colorMode == 0) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("BAR_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::Color1, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::Color1, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::Color1, 0).c_str(), 400);
	} else if (colorMode == 1) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("TOP_BG_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::Color2, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::Color2, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::Color2, 0).c_str(), 400);
	} else if (colorMode == 2) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("BOTTOM_BG_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::Color3, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::Color3, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::Color3, 0).c_str(), 400);
	} else if (colorMode == 3) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("TEXT_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::TxtColor, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::TxtColor, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::TxtColor, 0).c_str(), 400);
	} else if (colorMode == 4) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("SELECTED_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::SelectedColor, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::SelectedColor, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::SelectedColor, 0).c_str(), 400);
	} else if (colorMode == 5) {
		Gui::DrawStringCentered(0, 60, 0.7f, Config::TxtColor, Lang::get("UNSELECTED_COLOR"), 320);
		Gui::DrawString(40, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::UnselectedColor, 2).c_str(), 400);
		Gui::DrawString(140, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::UnselectedColor, 1).c_str(), 400);
		Gui::DrawString(245, 98, 0.7f, Config::TxtColor, ColorHelper::getColorName(Config::UnselectedColor, 0).c_str(), 400);
	}
}

void Settings::LanguageSelection(u32 hDown, touchPosition touch) {
	if (hDown & KEY_TOUCH) {
		for (int language = 0; language < 10; language++) {
			if (touching(touch, langBlocks[language])) {
				Config::lang = language;
				Lang::load(Config::lang);
				Config::saveConfig();
			}
		}
	}

	if (hDown & KEY_B) {
		Gui::screenBack();
		return;
	}
}

void Settings::colorChanging(u32 hDown, touchPosition touch) {
	int red;
	int green;
	int blue;

	if (hDown & KEY_B) {
		Config::saveConfig();
		Gui::screenBack();
		return;
	}

	if (hDown & KEY_L || hDown & KEY_LEFT) {
		if(colorMode > 0)	colorMode--;
	}

	if (hDown & KEY_R || hDown & KEY_RIGHT) {
		if(colorMode < 5)	colorMode++;
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
				}
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
				}
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
				}
			}
		}
	}
}


void Settings::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (mode == 0) {
		LanguageSelection(hDown, touch);
	} else if (mode == 1) {
		colorChanging(hDown, touch);
	}
}