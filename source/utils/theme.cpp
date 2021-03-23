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

#include "theme.hpp"
#include <regex>
#include <unistd.h>

/**
 * @brief Creates a 8 byte RGBA color
 * @param r red component of the color
 * @param g green component of the color
 * @param b blue component of the color
 * @param a alpha component of the color
 */
#define RGBA8(r, g, b, a) ((((r)&0xFF)<<0) | (((g)&0xFF)<<8) | (((b)&0xFF)<<16) | (((a)&0xFF)<<24))


Theme::Theme(const std::string &ThemeJSON) {
	/* Set Default colors. */
	this->vBarColor	         = C2D_Color32(50, 73, 98, 255);
	this->vBGColor	         = C2D_Color32(38, 44, 77, 255);
	this->vBarOutline        = C2D_Color32(25, 30, 53, 255);
	this->vTextColor         = C2D_Color32(255, 255, 255, 255);
	this->vEntryBar	         = C2D_Color32(50, 73, 98, 255);
	this->vEntryOutline      = C2D_Color32(25, 30, 53, 255);
	this->vBoxInside	     = C2D_Color32(28, 33, 58, 255);
	this->vBoxSelected	     = C2D_Color32(108, 130, 155, 255);
	this->vBoxUnselected     = C2D_Color32(0, 0, 0, 255);
	this->vProgressbarOut    = C2D_Color32(28, 33, 58, 255);
	this->vProgressbarIn     = C2D_Color32(77, 101, 128, 255);
	this->vSearchBar	     = C2D_Color32(51, 75, 102, 255);
	this->vSearchBarOutline  = C2D_Color32(25, 30, 53, 255);
	this->vSideBarSelected   = C2D_Color32(108, 130, 155, 255);
	this->vSideBarUnselected = C2D_Color32(77, 101, 128, 255);
	this->vMarkSelected		 = C2D_Color32(77, 101, 128, 255);
	this->vMarkUnselected	 = C2D_Color32(28, 33, 58, 255);
	this->vDownListPrev		 = C2D_Color32(28, 33, 58, 255);
	this->vSideBarIconColor  = C2D_Color32(173, 204, 239, 255);

	if (access(ThemeJSON.c_str(), F_OK) != 0) this->InitWithDefaultColors();

	FILE *file = fopen(ThemeJSON.c_str(), "r");
	this->json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
	this->Loaded = true;
}

void Theme::InitWithDefaultColors(const std::string &ThemePath) {
	const std::vector<std::string> DefaultColors = { // Default Universal-Updater Theme.
		"#324962", "#262C4D", "#191E35", "#FFFFFF",
		"#324962", "#191E35", "#1C213A", "#6C829B",
		"#000000", "#1C213A", "#4D6580", "#334B66",
		"#191E35", "#6C829B", "#4D6580", "#4D6580",
		"#1C213A", "#1C213A", "#ADCCEF"
	};

	nlohmann::json JS = { };
	JS["Default"] = DefaultColors;

	FILE *out = fopen(ThemePath.c_str(), "w");
	const std::string dump = JS.dump(1, '\t');
	fwrite(dump.c_str(), 1, JS.dump(1, '\t').size(), out);
	fclose(out);
}


void Theme::LoadTheme(const std::string &ThemeName) {
	if (this->Loaded && this->json.contains(ThemeName) && this->json[ThemeName].is_array()) {
		if (this->json[ThemeName].size() == 19) { // 19 Colors array.
			const std::vector<std::string> Colors = this->json[ThemeName].get<std::vector<std::string>>();

			this->vBarColor	         = this->GetThemeColor(Colors[0], C2D_Color32(50, 73, 98, 255));
			this->vBGColor	         = this->GetThemeColor(Colors[1], C2D_Color32(38, 44, 77, 255));
			this->vBarOutline        = this->GetThemeColor(Colors[2], C2D_Color32(25, 30, 53, 255));
			this->vTextColor         = this->GetThemeColor(Colors[3], C2D_Color32(255, 255, 255, 255));
			this->vEntryBar	         = this->GetThemeColor(Colors[4], C2D_Color32(50, 73, 98, 255));
			this->vEntryOutline      = this->GetThemeColor(Colors[5], C2D_Color32(25, 30, 53, 255));
			this->vBoxInside	     = this->GetThemeColor(Colors[6], C2D_Color32(28, 33, 58, 255));
			this->vBoxSelected	     = this->GetThemeColor(Colors[7], C2D_Color32(108, 130, 155, 255));
			this->vBoxUnselected     = this->GetThemeColor(Colors[8], C2D_Color32(0, 0, 0, 255));
			this->vProgressbarOut    = this->GetThemeColor(Colors[9], C2D_Color32(28, 33, 58, 255));
			this->vProgressbarIn     = this->GetThemeColor(Colors[10], C2D_Color32(77, 101, 128, 255));
			this->vSearchBar	     = this->GetThemeColor(Colors[11], C2D_Color32(51, 75, 102, 255));
			this->vSearchBarOutline  = this->GetThemeColor(Colors[12], C2D_Color32(25, 30, 53, 255));
			this->vSideBarSelected   = this->GetThemeColor(Colors[13], C2D_Color32(108, 130, 155, 255));
			this->vSideBarUnselected = this->GetThemeColor(Colors[14], C2D_Color32(77, 101, 128, 255));
			this->vMarkSelected		 = this->GetThemeColor(Colors[15], C2D_Color32(77, 101, 128, 255));
			this->vMarkUnselected	 = this->GetThemeColor(Colors[16], C2D_Color32(28, 33, 58, 255));
			this->vDownListPrev		 = this->GetThemeColor(Colors[17], C2D_Color32(28, 33, 58, 255));
			this->vSideBarIconColor  = this->GetThemeColor(Colors[18], C2D_Color32(173, 204, 239, 255));
		}
	}
}

std::vector<std::string> Theme::ThemeNames() {
	std::vector<std::string> Temp = { };

	if (this->Loaded) {
		for(auto it = this->json.begin(); it != this->json.end(); ++it) {
			Temp.push_back(it.key().c_str());
		}
	}

	return Temp;
}


uint32_t Theme::GetThemeColor(const std::string &colorString, const uint32_t DefaultColor) {
	if (colorString.length() < 7 || std::regex_search(colorString.substr(1), std::regex("[^0-9A-F]"))) { // invalid color.
		return DefaultColor;
	}

	int r = std::stoi(colorString.substr(1, 2), nullptr, 16);
	int g = std::stoi(colorString.substr(3, 2), nullptr, 16);
	int b = std::stoi(colorString.substr(5, 2), nullptr, 16);
	return RGBA8(r, g, b, 0xFF);
}