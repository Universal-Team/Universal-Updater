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

#include "rapidjson/document.h"
#include <citro2d.h>
#include <string>
#include <vector>

class Store {
public:
	enum class UpdateMode {
		automatic,
		forced,
		skip,
		spritesheet,
		header
	};

	struct Info {
		bool valid;
		std::string error;
		int entryCount;

		std::string title;
		std::string author;
		std::string description;
		int revision;

		std::string file;
		std::string url;
		std::string infoUrl;
		std::vector<std::string> sheets;
		std::vector<std::string> sheetUrls;

		int bgIndex = -1;
		int bgSheet = 0;
	};

	Store(const std::string &file, const std::string &fileName, UpdateMode updateMode);
	~Store();
	void LoadFromFile(const std::string &file, bool loadContent);
	void loadSheets();
	void unloadSheets();
	void update(const std::string &file, UpdateMode updateMode);

	const Store::Info &GetInfo() const { return this->info; }

	int GetStoreSize() const { return this->info.entryCount; }

	int GetScreenIndx() const { return this->screenIndex; };
	void SetScreenIndx(int v) { this->screenIndex = v; };

	int GetAnimOffset() const { return this->animOffset; };
	void SetAnimOffset(int v) { this->animOffset = v; };

	int GetEntry() const { return this->entry; };
	void SetEntry(int v) {
		this->entry = v;

		// Also reset download list
		this->downScrollOffset = 0;
		this->downIndex = 0;
	};

	int GetBox() const { return this->box; };
	void SetBox(int v) { this->box = v; };

	int GetDownloadScrollOffset() const { return this->downScrollOffset; };
	void SetDownloadScrollOffset(int v) { this->downScrollOffset = v; };

	int GetDownloadIndex() const { return this->downIndex; };
	void SetDownloadIndex(int v) { this->downIndex = v; };

	bool GetValid() const { return this->info.valid; };

	/* Both of these things are used for custom BG support. */
	C2D_Image GetStoreImg() const { return this->storeBG; };
	bool customBG() const { return this->hasCustomBG; };

	/* For getting a list of all categories and consoles used in the store. */
	const std::vector<std::string> &GetCategories() const { return this->categories; }
	const std::vector<std::string> &GetConsoles() const { return this->consoles; }

	bool GetIconValid(int iconIndex, int sheetIndex) const;
	C2D_Image GetIconEntry(int iconIndex, int sheetIndex) const;

private:
	bool LoadStoreInfo(const rapidjson::Value &storeInfo);
	bool LoadStoreContent(const rapidjson::Value &storeContent);

	void SetC2DBGImage();
	std::vector<std::string> categories;
	std::vector<std::string> consoles;
	std::vector<C2D_SpriteSheet> sheets;
	C2D_Image storeBG = { nullptr };
	bool hasSheet = false, hasCustomBG = false;
	int screenIndex = 0, entry = 0, box = 0, downIndex = 0, downScrollOffset = 0, animOffset = 0;
	Info info;
};

#endif
