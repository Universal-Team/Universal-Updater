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

#include "Common.hpp"
#include "ThemeData.hpp"
#include <regex>
#include <unistd.h>

/*
	Creates a 8 byte RGBA color.

	r: red component of the color.
	g: green component of the color.
	b: blue component of the color.
	a: alpha component of the color.
*/
#define RGBA8(r, g, b, a) ((((r) & 0xFF) << 0) | (((g) & 0xFF) << 8) | (((b) & 0xFF) << 16) | (((a) & 0xFF) << 24))



ThemeData::ThemeData() {
	FILE *File = fopen(THEME_JSON, "rt");

	if (File) this->ThemeJSON = nlohmann::json::parse(File, nullptr, false);

	if (!File || this->ThemeJSON.is_discarded()) this->ThemeJSON = this->InitWithDefaultColors();
	if (File) fclose(File);
	this->Loaded = true;
};


/*
	Initializes the Theme Data with default colors.
*/
nlohmann::json ThemeData::InitWithDefaultColors() {
	nlohmann::json JS = {
		{
			"Default", {
				{ "BarColor", "#324962" },
				{ "BGColor", "#262C4D" },
				{ "BarOutline", "#191E35" },
				{ "TextColor", "#FFFFFF" },
				{ "EntryBar", "#324962" },
				{ "EntryOutline", "#191E35" },
				{ "BoxInside", "#1C213A" },
				{ "BoxSelected", "#6C829B" },
				{ "BoxUnselected", "#000000" },
				{ "ProgressbarOut", "#1C213A" },
				{ "ProgressbarIn", "#4D6580" },
				{ "SearchBar", "#334B66" },
				{ "SearchBarOutline", "#191E35" },
				{ "SideBarSelected", "#6C829B" },
				{ "SideBarUnselected", "#4D6580" },
				{ "MarkSelected", "#4D6580" },
				{ "MarkUnselected", "#1C213A" },
				{ "DownListPrev", "#1C213A" },
				{ "SideBarIconColor", "#ADCCEF" },
				{ "Description", "Universal-Updater's default Theme.\n\nBy: Universal-Team" }
			}
		}
	};

	FILE *Out = fopen(THEME_JSON, "w");

	if (Out) {
		const std::string Dump = JS.dump(1, '\t');
		fwrite(Dump.c_str(), 1, JS.dump(1, '\t').size(), Out);
		fclose(Out);
	}

	return JS;
};


/*
	Loads a theme.

	const std::string &ThemeName: The name of the theme to load.
*/
void ThemeData::LoadTheme(const std::string &ThemeName) {
	this->vBarColor	         = this->GetThemeColor(ThemeName, "BarColor", RGBA8(50, 73, 98, 255));
	this->vBGColor	         = this->GetThemeColor(ThemeName, "BGColor", RGBA8(38, 44, 77, 255));
	this->vBarOutline        = this->GetThemeColor(ThemeName, "BarOutline", RGBA8(25, 30, 53, 255));
	this->vTextColor         = this->GetThemeColor(ThemeName, "TextColor", RGBA8(255, 255, 255, 255));
	this->vEntryBar	         = this->GetThemeColor(ThemeName, "EntryBar", RGBA8(50, 73, 98, 255));
	this->vEntryOutline      = this->GetThemeColor(ThemeName, "EntryOutline", RGBA8(25, 30, 53, 255));
	this->vBoxInside	     = this->GetThemeColor(ThemeName, "BoxInside", RGBA8(28, 33, 58, 255));
	this->vBoxSelected	     = this->GetThemeColor(ThemeName, "BoxSelected", RGBA8(108, 130, 155, 255));
	this->vBoxUnselected     = this->GetThemeColor(ThemeName, "BoxUnselected", RGBA8(0, 0, 0, 255));
	this->vProgressbarOut    = this->GetThemeColor(ThemeName, "ProgressbarOut", RGBA8(28, 33, 58, 255));
	this->vProgressbarIn     = this->GetThemeColor(ThemeName, "ProgressbarIn", RGBA8(77, 101, 128, 255));
	this->vSearchBar	     = this->GetThemeColor(ThemeName, "SearchBar", RGBA8(51, 75, 102, 255));
	this->vSearchBarOutline  = this->GetThemeColor(ThemeName, "SearchBarOutline", RGBA8(25, 30, 53, 255));
	this->vSideBarSelected   = this->GetThemeColor(ThemeName, "SideBarSelected", RGBA8(108, 130, 155, 255));
	this->vSideBarUnselected = this->GetThemeColor(ThemeName, "SideBarUnselected", RGBA8(77, 101, 128, 255));
	this->vMarkSelected		 = this->GetThemeColor(ThemeName, "MarkSelected", RGBA8(77, 101, 128, 255));
	this->vMarkUnselected	 = this->GetThemeColor(ThemeName, "MarkUnselected", RGBA8(28, 33, 58, 255));
	this->vDownListPrev		 = this->GetThemeColor(ThemeName, "DownListPrev", RGBA8(28, 33, 58, 255));
	this->vSideBarIconColor  = this->GetThemeColor(ThemeName, "SideBarIconColor", RGBA8(173, 204, 239, 255));
};


/*
	Return a std::vector of pairs with all the Theme names and descriptions.
*/
std::vector<std::pair<std::string, std::string>> ThemeData::ThemeNames() {
	std::vector<std::pair<std::string, std::string>> Temp = { };

	if (this->Loaded) {
		for(auto It = this->ThemeJSON.begin(); It != this->ThemeJSON.end(); ++It) {
			if (this->ThemeJSON[It.key()].contains("Description") && this->ThemeJSON[It.key()]["Description"].is_string()) {
				Temp.push_back(std::make_pair(It.key(), this->ThemeJSON[It.key()]["Description"]));

			} else {
				Temp.push_back(std::make_pair(It.key(), It.key()));
			}
		}
	}

	return Temp;
};


/*
	Handle the color return from the JSON.

	const std::string &ThemeName: The name of the Theme.
	const std::string &Key: The color "key".
	const uint32_t DefaultColor: The default color to return.
*/
uint32_t ThemeData::GetThemeColor(const std::string &ThemeName, const std::string &Key, const uint32_t DefaultColor) {
	if (this->ThemeJSON.contains(ThemeName) && this->ThemeJSON[ThemeName].is_object() && this->ThemeJSON[ThemeName].contains(Key) && this->ThemeJSON[ThemeName][Key].is_string()) {
		const std::string ColorString = this->ThemeJSON[ThemeName][Key].get_ref<const std::string &>();

		if (ColorString.length() < 7 || std::regex_search(ColorString.substr(1), std::regex("[^0-9A-Fa-f]"))) { // invalid color.
			return DefaultColor;
		}

		#ifdef __3DS__
			const int R = std::stoi(ColorString.substr(1, 2), nullptr, 16);
			const int G = std::stoi(ColorString.substr(3, 2), nullptr, 16);
			const int B = std::stoi(ColorString.substr(5, 2), nullptr, 16);
			return RGBA8(R, G, B, 0xFF);

		#elif ARM9
			/* TODO: Handle BGR15 conversion. */
		#endif

	}

	return DefaultColor;
};