// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

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
		std::string File;
		std::string Description;
		int Version;
		int Revision;
		int StoreSize;
	};

	static constexpr int UNISTORE_VERSION = 4;
	static Info GetInfo(const std::string &FName);
	static Info GetInfo(const nlohmann::json &JSON);
	static std::vector<Info> GetUniStoreInfo(const std::string &Path);

	UniStore(const std::string &FileName);
	~UniStore();

	/* Some Utilities. */
	void Update(bool Force);
	void LoadSpriteSheets();
	void ResetIndexes();

	/* Here get UniStore related things. */
	const std::string &GetUniStoreTitle() const;
	bool UniStoreValid() const { return this->Valid; };

	/* Entry related Info. */
	const std::string &GetEntryTitle(const int Idx) const;
	const std::string &GetEntryAuthor(const int Idx) const;
	const std::string &GetEntryDescription(const int Idx) const;
	std::vector<std::string> GetEntryCategories(const int Idx) const;
	const std::string &GetEntryVersion(const int Idx) const;
	std::vector<std::string> GetEntryConsoles(const int Idx) const;
	const std::string &GetEntryLastUpdated(const int Idx) const;
	const std::string &GetEntryLicense(const int Idx) const;
	int GetEntryIcon(const int Idx) const;
	int GetEntrySheet(const int Idx) const;

	/* Some other returns. */
	std::vector<std::string> GetDownloadList(const int Idx) const;
	const std::string &GetFileSizes(const int Idx, const std::string &Entry) const;
	std::vector<std::string> GetScreenshotList(const int Idx) const;
	std::vector<std::string> GetScreenshotNames(const int Idx) const;
	const std::string &GetReleaseNotes(const int Idx) const;
	const nlohmann::json *GetScript(const int Idx, const std::string &Script) const;

	std::vector<size_t> Indexes;
	size_t SelectedIndex = 0, ScreenIndex = 0;
private:
	static const std::string emptyString;

	std::string FileName = "";
	bool Valid = false;
	nlohmann::json UniStoreJSON = nullptr;

	void Load();
};

#endif