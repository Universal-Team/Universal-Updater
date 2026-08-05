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
#include "common.hpp"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include <regex>
#include <unistd.h>

using namespace rapidjson;

Theme::Theme(const std::string &name) : vName(name) {
	FILE *in = fopen(_THEME_PATH, "r");
	if (!in) return;

	Document json;
	char *readBuffer = new char[0x10000];
	FileReadStream is(in, readBuffer, 0x10000);
	json.ParseStream(is);
	delete[] readBuffer;
	fclose(in);

	if (!json.IsObject()) return;

	if (json.HasMember(vName.c_str()))
		Load(json[vName.c_str()]);
}

void Theme::Load(const Value &json) {
	this->vBarColor          = this->GetThemeColor(json, "BarColor", this->vBarColor);
	this->vBGColor           = this->GetThemeColor(json, "BGColor", this->vBGColor);
	this->vBarOutline        = this->GetThemeColor(json, "BarOutline", this->vBarOutline);
	this->vTextColor         = this->GetThemeColor(json, "TextColor", this->vTextColor);
	this->vEntryBar          = this->GetThemeColor(json, "EntryBar", this->vEntryBar);
	this->vEntryOutline      = this->GetThemeColor(json, "EntryOutline", this->vEntryOutline);
	this->vBoxInside         = this->GetThemeColor(json, "BoxInside", this->vBoxInside);
	this->vBoxSelected       = this->GetThemeColor(json, "BoxSelected", this->vBoxSelected);
	this->vBoxUnselected     = this->GetThemeColor(json, "BoxUnselected", this->vBoxUnselected);
	this->vProgressbarOut    = this->GetThemeColor(json, "ProgressbarOut", this->vProgressbarOut);
	this->vProgressbarIn     = this->GetThemeColor(json, "ProgressbarIn", this->vProgressbarIn);
	this->vSearchBar         = this->GetThemeColor(json, "SearchBar", this->vSearchBar);
	this->vSearchBarOutline  = this->GetThemeColor(json, "SearchBarOutline", this->vSearchBarOutline);
	this->vSideBarSelected   = this->GetThemeColor(json, "SideBarSelected", this->vSideBarSelected);
	this->vSideBarUnselected = this->GetThemeColor(json, "SideBarUnselected", this->vSideBarUnselected);
	this->vMarkSelected      = this->GetThemeColor(json, "MarkSelected", this->vMarkSelected);
	this->vMarkUnselected    = this->GetThemeColor(json, "MarkUnselected", this->vMarkUnselected);
	this->vDownListPrev      = this->GetThemeColor(json, "DownListPrev", this->vDownListPrev);
	this->vSideBarIconColor  = this->GetThemeColor(json, "SideBarIconColor", this->vSideBarIconColor);

	if (json.HasMember("Description") && json["Description"].IsString()) this->vDescription = json["Description"].GetString();
}

bool Theme::AddToJson() const {
	Document json;
	bool changesMade = false;

	FILE *in = fopen(_THEME_PATH, "r");
	if (in) {
		char *readBuffer = new char[0x10000];
		FileReadStream is(in, readBuffer, 0x10000);
		json.ParseStream(is);
		delete[] readBuffer;
		fclose(in);

		if (!json.IsObject()) return false;
	} else {
		json.SetObject();
	}

	if (json.HasMember(vName.c_str())) {
		if (!json[vName.c_str()].IsObject()) {
			json[vName.c_str()].SetObject();
			changesMade = true;
		}
	} else {
		json.AddMember(Value(vName.c_str(), vName.size()), Value(kObjectType).Move(), json.GetAllocator());
		changesMade = true;
	}

	Value &theme = json[vName.c_str()];
	changesMade |= this->SetThemeColor(theme, "BarColor", this->vBarColor, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "BGColor", this->vBGColor, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "BarOutline", this->vBarOutline, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "TextColor", this->vTextColor, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "EntryBar", this->vEntryBar, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "EntryOutline", this->vEntryOutline, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "BoxInside", this->vBoxInside, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "BoxSelected", this->vBoxSelected, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "BoxUnselected", this->vBoxUnselected, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "ProgressbarOut", this->vProgressbarOut, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "ProgressbarIn", this->vProgressbarIn, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "SearchBar", this->vSearchBar, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "SearchBarOutline", this->vSearchBarOutline, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "SideBarSelected", this->vSideBarSelected, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "SideBarUnselected", this->vSideBarUnselected, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "MarkSelected", this->vMarkSelected, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "MarkUnselected", this->vMarkUnselected, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "DownListPrev", this->vDownListPrev, json.GetAllocator());
	changesMade |= this->SetThemeColor(theme, "SideBarIconColor", this->vSideBarIconColor, json.GetAllocator());

	if(changesMade) {
		FILE *out = fopen(_THEME_PATH, "w");
		if (out) {
			char *writeBuffer = new char[0x10000];
			FileWriteStream os(out, writeBuffer, 0x10000);

			PrettyWriter<FileWriteStream> writer(os);
			writer.SetIndent('\t', 1);
			json.Accept(writer);

			delete[] writeBuffer;
			fclose(out);
		}

		return true;
	}

	return false;
}

uint32_t Theme::GetThemeColor(const Value &json, const char *Key, const uint32_t DefaultColor) const {
	if (!json.IsObject() || !json.HasMember(Key) || !json[Key].IsString()) return DefaultColor;

	const std::string &colorString = json[Key].GetString();
	if (colorString.length() < 7 || std::regex_search(colorString.substr(1), std::regex("[^0-9A-Fa-f]"))) { // invalid color.
		return DefaultColor;
	}

	int r = std::stoi(colorString.substr(1, 2), nullptr, 16);
	int g = std::stoi(colorString.substr(3, 2), nullptr, 16);
	int b = std::stoi(colorString.substr(5, 2), nullptr, 16);
	return C2D_Color32(r, g, b, 0xFF);
}

bool Theme::SetThemeColor(Value &json, const char *Key, const uint32_t Color, Document::AllocatorType &a) const {
	if (!json.IsObject()) return false;

	char colorString[8];
	snprintf(colorString, sizeof(colorString), "#%02X%02X%02X", (int)(Color & 0xFF), (int)(Color >> 8) & 0xFF, (int)(Color >> 16) & 0xFF);

	if (json.HasMember(Key)) {
		if(!json[Key].IsString() || strcmp(json[Key].GetString(), colorString) != 0) {
			json[Key].SetString(colorString, 7, a);
			return true;
		}
	} else {
		json.AddMember(Value(Key, strlen(Key)).Move(), Value().SetString(colorString, 7, a).Move(), a);
		return true;
	}

	return false;
}