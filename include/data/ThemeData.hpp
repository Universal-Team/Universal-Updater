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

#ifndef _UNIVERSAL_UPDATER_THEME_DATA_HPP
#define _UNIVERSAL_UPDATER_THEME_DATA_HPP

#include "JSON.hpp"
#include <string>


/* We're forced to handle it like this, because we cannot include UniversalUpdater.hpp here properly. */
#ifdef _3DS
	typedef uint32_t ThemeColor; // RGBA8.
#elif ARM9
	typedef uint16_t ThemeColor; // BGR15.
#endif


class ThemeData {
public:
	ThemeData();
	nlohmann::json InitWithDefaultColors();
	void LoadTheme(const std::string &ThemeName);
	std::vector<std::pair<std::string, std::string>> ThemeNames();
	
	ThemeColor BarColor() const { return this->vBarColor; };
	ThemeColor BGColor() const { return this->vBGColor; };
	ThemeColor BarOutline() const { return this->vBarOutline; };
	ThemeColor TextColor() const { return this->vTextColor; };
	ThemeColor EntryBar() const { return this->vEntryBar; };
	ThemeColor EntryOutline() const { return this->vEntryOutline; };
	ThemeColor BoxInside() const { return this->vBoxInside; };
	ThemeColor BoxSelected() const { return this->vBoxSelected; };
	ThemeColor BoxUnselected() const { return this->vBoxUnselected; };
	ThemeColor ProgressbarOut() const { return this->vProgressbarOut; };
	ThemeColor ProgressbarIn() const { return this->vProgressbarIn; };
	ThemeColor SearchBar() const { return this->vSearchBar; };
	ThemeColor SearchBarOutline() const { return this->vSearchBarOutline; };
	ThemeColor SideBarSelected() const { return this->vSideBarSelected; };
	ThemeColor SideBarUnselected() const { return this->vSideBarUnselected; };
	ThemeColor MarkSelected() const { return this->vMarkSelected; };
	ThemeColor MarkUnselected() const { return this->vMarkUnselected; };
	ThemeColor DownListPrev() const { return this->vDownListPrev; };
	ThemeColor SideBarIconColor() const { return this->vSideBarIconColor; };
private:
	ThemeColor GetThemeColor(const std::string &ThemeName, const std::string &Key, const ThemeColor DefaultColor);

	ThemeColor vBarColor = 0, vBGColor = 0, vBarOutline = 0, vTextColor = 0, vEntryBar = 0, vEntryOutline = 0,
			 vBoxInside = 0, vBoxSelected = 0, vBoxUnselected = 0, vProgressbarOut = 0, vProgressbarIn = 0,
			 vSearchBar = 0, vSearchBarOutline = 0, vSideBarSelected = 0, vSideBarUnselected = 0,
			 vMarkSelected = 0, vMarkUnselected = 0, vDownListPrev = 0, vSideBarIconColor = 0;

	int SelectedTheme = 0;
	bool Loaded = false;
	nlohmann::json ThemeJSON = nullptr;
};

#endif