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

#ifndef _UNIVERSAL_UPDATER_UNISTORE_HPP
#define _UNIVERSAL_UPDATER_UNISTORE_HPP

#include "JSON.hpp"
#include <string>
#include <vector>


class UniStore {
public:
	struct Info {
		std::string Title;
		std::string Author;
		std::string URL;
		std::string File; // Used to check, if File does NOT contain a slash or so.
		std::string FileName;
		std::string Description;
		int Version;
		int Revision;
		int StoreSize;
	};

	static constexpr int UNISTORE_VERSION = 4;
	static bool FirstStart;
	static Info GetInfo(const std::string &File, const std::string &FName);
	static std::vector<Info> GetUniStoreInfo(const std::string &Path);

	UniStore(const std::string &FullPath, const std::string &FileName, const bool DidDownload = false);
	~UniStore();

	/* Some Utilities. */
	void UpdateUniStore(const std::string &File);
	void LoadUniStore(const std::string &File, const bool FullInit = false);
	void LoadSpriteSheets();
	void ResetIndexes();

	/* Here get UniStore related things. */
	std::string GetUniStoreTitle() const;
	bool UniStoreValid() const { return this->Valid; };

	/* Entry related Info. */
	std::string GetEntryTitle(const int Idx) const;
	std::string GetEntryAuthor(const int Idx) const;
	std::string GetEntryDescription(const int Idx) const;
	std::vector<std::string> GetEntryCategories(const int Idx) const;
	std::string GetEntryVersion(const int Idx) const;
	std::vector<std::string> GetEntryConsoles(const int Idx) const;
	std::string GetEntryLastUpdated(const int Idx) const;
	std::string GetEntryLicense(const int Idx) const;
	int GetEntryIcon(const int Idx) const;
	int GetEntrySheet(const int Idx) const;

	/* Some other returns. */
	std::vector<std::string> GetDownloadList(const int Idx) const;
	std::string GetFileSizes(const int Idx, const std::string &Entry) const;
	std::vector<std::string> GetScreenshotList(const int Idx) const;
	std::vector<std::string> GetScreenshotNames(const int Idx) const;
	std::string GetReleaseNotes(const int Idx) const;
	const nlohmann::json *GetScript(const int Idx, const std::string &Script) const;

	std::vector<size_t> Indexes;
	size_t SelectedIndex = 0, ScreenIndex = 0;
private:
	std::string FileName = "";
	bool Valid = false;
	nlohmann::json UniStoreJSON = nullptr;
};

#endif