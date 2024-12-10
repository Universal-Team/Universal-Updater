// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_BROWSE_DATA_HPP
#define _UNIVERSAL_UPDATER_BROWSE_DATA_HPP

#include <string>
#include <vector>


class BrowseData {
public:
	/* Directory Struct. */
	struct DirEntry {
		std::string Name;
		std::string Path;
		bool IsDirectory;
	};

	BrowseData(const std::string &Path, const std::vector<std::string> &Extensions = std::vector<std::string>({ "" }));
	BrowseData(const std::vector<std::string> &List);

	/* Actions. */
	void Up();
	void Down();
	void Right(const int Amount);
	void Left(const int Amount);
	bool OpenHandle();
	bool CanDirBack();
	void GoDirBack();
	void GoDirUp();
	bool SetSelection(const int Selection);
	void RefreshList();

	/* Getters. */
	std::vector<std::string> GetList() { return (this->Type == BrowserType::File ? this->GetFileList() : this->ListEntries); };
	std::vector<DirEntry> GetDirEntries() const { return this->DirEntries; };
	int GetSelectedIndex() const { return this->Selected; };
	std::string GetSelectedName() const { return (this->Type == BrowserType::File ? this->DirEntries[this->Selected].Name : this->ListEntries[this->Selected]); };
	std::string GetSelectedPath() const { return (this->Type == BrowserType::File ? (this->GetPath() + this->DirEntries[this->Selected].Name) : ""); };
	bool GetRefresh() const { return this->Refresh; };
	bool GetEmpty() const { return (this->Type == BrowserType::File ? this->DirEntries.empty() : this->ListEntries.empty()); };
	std::string GetPath() const;
private:
	/* Browser Types. */
	enum class BrowserType : uint8_t { File, List };

	int Selected = 0;
	bool Refresh = false;
	BrowserType Type = BrowserType::File;
	std::vector<DirEntry> DirEntries = { }; // Only used in File mode.
	std::vector<std::string> ListEntries = { }; // Only used in List mode.
	std::vector<std::string> ExtensionList; // Well, the Extensions for File mode.

	bool NameEndsWith(const std::string &Name, const std::vector<std::string> &ExtensionList) const;
	std::vector<std::string> GetFileList() const;
	void FetchDirectoryEntries(const std::vector<std::string> &ExtList = { });
};

#endif