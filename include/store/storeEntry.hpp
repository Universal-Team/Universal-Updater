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

#ifndef _UNIVERSAL_UPDATER_STORE_ENTRY_HPP
#define _UNIVERSAL_UPDATER_STORE_ENTRY_HPP

#include "meta.hpp"
#include "rapidjson/document.h"
#include "script.hpp"
#include "store.hpp"
#include "stringutils.hpp"
#include <citro2d.h>

class StoreEntry {
public:
	StoreEntry(const rapidjson::Value &json, const Store &store);

	const std::string &GetTitle() const { return this->Title; };
	const std::string &GetAuthor() const { return this->Author; };
	const std::string &GetDescription() const { return this->Description; };
	const std::string &GetVersion() const { return this->Version; };
	const std::string &GetCategoryString() const { return this->Category; };
	const std::string &GetConsoleString() const { return this->Console; };
	const std::string &GetLastUpdated() const { return this->LastUpdated; };
	int GetStars() const { return this->Stars; };
	const std::string &GetLicense() const { return this->License; };
	const std::string &GetLlmGeneration() const { return this->LlmGeneration; };
	const std::string &GetWiki() const { return this->Wiki; };
	const std::string &GetPreinstallMessage() const { return this->PreinstallMessage; };
	const std::vector<std::string> &GetInstalledFiles(int index) const { return this->InstalledFiles; };
	const std::vector<int> &GetTitleIds(int index) const { return this->TitleIds; };
	int GetMarks() const { return this->Marks; };
	const std::string &GetMarkString() const { return this->MarkString; }
	const std::string &GetUniStore() const { return this->UniStore; };

	bool HasIcon() const { return this->vHasIcon; }
	C2D_Image GetIcon() const { return this->Icon; };

	int GetSheetIndex() const { return this->SheetIndex; };

	uint32_t GetAccentColor() const { return this->AccentColor; };

	Script &GetScript(int i) { return this->Scripts[i]; };
	std::vector<Script> &GetScripts() { return this->Scripts; };
	const std::vector<std::string> &GetCategories() const { return this->FullCategory; };
	const std::vector<std::string> &GetConsoles() const { return this->FullConsole; };
	const std::vector<std::string> &GetScreenshots() const { return this->Screenshots; };
	const std::vector<std::string> &GetScreenshotNames() const { return this->ScreenshotNames; };
	const std::string &GetReleaseNotes() const { return this->ReleaseNotes; };

	bool GetUpdateAvl() const { return this->UpdateAvailable; };
	void SetUpdateAvl(bool v) { this->UpdateAvailable = v; };

	bool CheckInstalled() const;
	bool GetInstalled() const { return this->Installed; }
	void SetInstalled(bool v) { this->Installed = v; }

	void SetMark(int marks) {
		this->Marks = marks;
		this->MarkString = StringUtils::GetMarkString(this->Marks);
	};

private:
	std::string Title, Author, Description, Category, Version, Console, LastUpdated, License, LlmGeneration, MarkString, ReleaseNotes, Wiki, PreinstallMessage, UniStore;
	int Stars = 0;
	C2D_Image Icon;
	int SheetIndex = 0, Marks = 0;
	uint32_t AccentColor = 0;
	std::vector<Script> Scripts;
	std::vector<std::string> FullCategory, FullConsole, Screenshots, ScreenshotNames, InstalledFiles;
	std::vector<int> TitleIds;
	bool UpdateAvailable = false, vHasIcon = false, Installed = false;
};

#endif