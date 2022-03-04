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

#ifndef _UNIVERSAL_UPDATER_STORE_HPP
#define _UNIVERSAL_UPDATER_STORE_HPP

#include "json.hpp"
#include <citro2d.h>
#include <string>

class Store {
public:
	Store(const std::string &file, const std::string &file2, bool ARGMode = false);
	~Store();
	void LoadFromFile(const std::string &file);
	void loadSheets();
	void unloadSheets();
	void update(const std::string &file);

	/* Get Information of the UniStore itself. */
	std::string GetUniStoreTitle() const;
	std::string GetUniStoreAuthor() const;

	/* Get Information of the UniStore entries. */
	std::string GetTitleEntry(int index) const;
	std::string GetAuthorEntry(int index) const;
	std::string GetDescriptionEntry(int index) const;
	std::vector<std::string> GetCategoryIndex(int index) const;
	std::string GetVersionEntry(int index) const;
	std::vector<std::string> GetConsoleEntry(int index) const;
	std::string GetLastUpdatedEntry(int index) const;
	std::string GetLicenseEntry(int index) const;
	C2D_Image GetIconEntry(int index) const;
	std::string GetFileSizes(int index, const std::string &entry) const;
	std::string GetFileTypes(int index, const std::string &entry) const;
	std::vector<std::string> GetScreenshotList(int index) const;
	std::vector<std::string> GetScreenshotNames(int index) const;
	std::string GetReleaseNotes(int index) const;

	std::vector<std::string> GetDownloadList(int index) const;

	int GetStoreSize() const { return (int)this->storeJson["storeContent"].size(); };

	int GetScreenIndx() const { return this->screenIndex; };
	void SetScreenIndx(int v) { this->screenIndex = v; };

	int GetEntry() const { return this->entry; };
	void SetEntry(int v) { this->entry = v; };

	int GetBox() const { return this->box; };
	void SetBox(int v) { this->box = v; };

	int GetDownloadSIndex() const { return this->downIndex; };
	void SetDownloadSIndex(int v) { this->downIndex = v; };

	int GetDownloadIndex() const { return this->downEntry; };
	void SetDownloadIndex(int v) { this->downEntry = v; };

	nlohmann::json &GetJson() { return this->storeJson; };
	bool GetValid() const { return this->valid; };

	/* Both of these things are used for custom BG support. */
	C2D_Image GetStoreImg() const { return this->storeBG; };
	bool customBG() const { return this->hasCustomBG; };

	/* Return filename of the UniStore. */
	std::string GetFileName() const { return this->fileName; };
private:
	void SetC2DBGImage();
	nlohmann::json storeJson = nullptr;
	std::vector<C2D_SpriteSheet> sheets;
	C2D_Image storeBG = { nullptr };
	bool valid = false, hasSheet = false, hasCustomBG = false;
	int screenIndex = 0, entry = 0, box = 0, downEntry = 0, downIndex = 0;
	std::string fileName = "";
};

#endif