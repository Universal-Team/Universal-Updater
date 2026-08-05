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

#ifndef _UNIVERSAL_UPDATER_THEME_HPP
#define _UNIVERSAL_UPDATER_THEME_HPP

#include "rapidjson/document.h"
#include <citro2d.h>
#include <string>

class Theme {
public:
	Theme() {}
	Theme(const std::string &name);
	Theme(const rapidjson::Value &json, const std::string &name) : vName(name) { Load(json); }

	bool AddToJson(void) const;

	uint32_t BarColor() const { return this->vBarColor; };
	uint32_t BGColor() const { return this->vBGColor; };
	uint32_t BarOutline() const { return this->vBarOutline; };
	uint32_t TextColor() const { return this->vTextColor; };
	uint32_t EntryBar() const { return this->vEntryBar; };
	uint32_t EntryOutline() const { return this->vEntryOutline; };
	uint32_t BoxInside() const { return this->vBoxInside; };
	uint32_t BoxSelected() const { return this->vBoxSelected; };
	uint32_t BoxUnselected() const { return this->vBoxUnselected; };
	uint32_t ProgressbarOut() const { return this->vProgressbarOut; };
	uint32_t ProgressbarIn() const { return this->vProgressbarIn; };
	uint32_t SearchBar() const { return this->vSearchBar; };
	uint32_t SearchBarOutline() const { return this->vSearchBarOutline; };
	uint32_t SideBarSelected() const { return this->vSideBarSelected; };
	uint32_t SideBarUnselected() const { return this->vSideBarUnselected; };
	uint32_t MarkSelected() const { return this->vMarkSelected; };
	uint32_t MarkUnselected() const { return this->vMarkUnselected; };
	uint32_t DownListPrev() const { return this->vDownListPrev; };
	uint32_t SideBarIconColor() const { return this->vSideBarIconColor; };

	const std::string &Name() const { return this->vName; }
	const std::string &Description() const { return this->vDescription; }

private:
	void Load(const rapidjson::Value &json);
	uint32_t GetThemeColor(const rapidjson::Value &json, const char *Key, const uint32_t DefaultColor) const;
	bool SetThemeColor(rapidjson::Value &json, const char *Key, const uint32_t Color, rapidjson::Document::AllocatorType &a) const;

	uint32_t vBarColor          = C2D_Color32( 50,  73,  98, 255),
			 vBGColor           = C2D_Color32( 38,  44,  77, 255),
			 vBarOutline        = C2D_Color32( 25,  30,  53, 255),
			 vTextColor         = C2D_Color32(255, 255, 255, 255),
			 vEntryBar          = C2D_Color32( 50,  73,  98, 255),
			 vEntryOutline      = C2D_Color32( 25,  30,  53, 255),
			 vBoxInside         = C2D_Color32( 28,  33,  58, 255),
			 vBoxSelected       = C2D_Color32(108, 130, 155, 255),
			 vBoxUnselected     = C2D_Color32(  0,   0,   0, 255),
			 vProgressbarOut    = C2D_Color32( 28,  33,  58, 255),
			 vProgressbarIn     = C2D_Color32( 77, 101, 128, 255),
			 vSearchBar         = C2D_Color32( 51,  75, 102, 255),
			 vSearchBarOutline  = C2D_Color32( 25,  30,  53, 255),
			 vSideBarSelected   = C2D_Color32(108, 130, 155, 255),
			 vSideBarUnselected = C2D_Color32( 77, 101, 128, 255),
			 vMarkSelected      = C2D_Color32( 77, 101, 128, 255),
			 vMarkUnselected    = C2D_Color32( 28,  33,  58, 255),
			 vDownListPrev      = C2D_Color32( 28,  33,  58, 255),
			 vSideBarIconColor  = C2D_Color32(173, 204, 239, 255);

	std::string vName, vDescription;
};

#endif