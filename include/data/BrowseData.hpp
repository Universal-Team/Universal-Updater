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