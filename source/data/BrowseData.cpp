// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "BrowseData.hpp"
#include <algorithm> // sort.
#include <cstring> // strcasecmp.
#include <dirent.h> // DIR.
#include <functional> // sort.
#include <unistd.h> // chdir.


/*
	Browser Initializer for a File / Directory type.

	const std::string &Path: The Start Path from where to load.
	const std::vector<std::string> &Extensions: The File Extensions which to filter (Optional).
*/
BrowseData::BrowseData(const std::string &Path, const std::vector<std::string> &Extensions) : ExtensionList(Extensions) {
	this->Type = BrowserType::File;

	chdir(Path.c_str());
	this->FetchDirectoryEntries(this->ExtensionList);
};


/*
	Browser Initializer for a List type.

	const std::vector<std::string> &List: The list of Strings for the List type.
*/
BrowseData::BrowseData(const std::vector<std::string> &List) : ListEntries(List) { this->Type = BrowserType::List; };


/*
	Return, if a specific Name ends with an Extension.

	const std::string &Name: The Filename to check.
	const std::vector<std::string> &ExtList: The Extension list which to check.
*/
bool BrowseData::NameEndsWith(const std::string &Name, const std::vector<std::string> &ExtList) const {
	if (Name.substr(0, 2) == "._") return false;
	if (Name.size() == 0) return false;
	if (ExtList.size() == 0) return true;

	for(int Idx = 0; Idx < (int)ExtList.size(); Idx++) {
		const std::string Ext = ExtList.at(Idx);
		if (strcasecmp(Name.c_str() + Name.size() - Ext.size(), Ext.c_str()) == 0) return true;
	}

	return false;
};


/*
	Predict File Entries Alphabetically.

	const BrowseData::DirEntry &First: The first Entry which to compare.
	const BrowseData::DirEntry &Second: The second Entry which to compare.
*/
bool DirEntryPredicate(const BrowseData::DirEntry &First, const BrowseData::DirEntry &Second) {
	if (!First.IsDirectory && Second.IsDirectory) return false;
	if (First.IsDirectory && !Second.IsDirectory) return true;

	return strcasecmp(First.Name.c_str(), Second.Name.c_str()) < 0;
};


/*
	Fetches Directory Contents.

	const std::vector<std::string> &ExtList: The extensions which to filter (Optional).
*/
void BrowseData::FetchDirectoryEntries(const std::vector<std::string> &ExtList) {
	struct stat Stat;
	this->DirEntries.clear();
	DIR *PDir = opendir(".");

	if (PDir) {
		while(1) {
			DirEntry DEntry;
			struct dirent *Pent = readdir(PDir);
			if (!Pent) break;

			/* Fetch the file name and directory state. */
			stat(Pent->d_name, &Stat);
			DEntry.Name = Pent->d_name;
			DEntry.IsDirectory = (Stat.st_mode & S_IFDIR) ? true : false;

			if (DEntry.Name.compare(".") != 0 && (DEntry.IsDirectory || this->NameEndsWith(DEntry.Name, (ExtList.empty() ? this->ExtensionList : ExtList)))) {
				this->DirEntries.push_back(DEntry);
			}
		}

		closedir(PDir);
	}

	std::sort(this->DirEntries.begin(), this->DirEntries.end(), DirEntryPredicate); // Sort this alphabetically.
	if (this->CanDirBack()) this->DirEntries.insert(this->DirEntries.begin(), { "..", "", false }); // Push a ".." for going back.
};


/* Returns the Directory Entry Files as a vector of strings. */
std::vector<std::string> BrowseData::GetFileList() const {
	std::vector<std::string> TMP = { };

	if (!this->DirEntries.empty()) {
		for (int Idx = 0; Idx < (int)this->DirEntries.size(); Idx++) TMP.push_back(this->DirEntries[Idx].Name);
	}

	return TMP;
};


/* Up, Down, Left and Right Callbacks. */
void BrowseData::Up() {
	switch(this->Type) {
		case BrowserType::File:
			if (this->DirEntries.empty()) break;
			if (this->Selected > 0) this->Selected--;
			else this->Selected = (int)this->DirEntries.size() - 1;
			break;

		case BrowserType::List:
			if (this->ListEntries.empty()) break;
			if (this->Selected > 0) this->Selected--;
			else this->Selected = (int)this->ListEntries.size() - 1;
			break;
	}
};


void BrowseData::Down() {
	switch(this->Type) {
		case BrowserType::File:
			if (this->DirEntries.empty()) break;
			if (this->Selected < (int)this->DirEntries.size() - 1) this->Selected++;
			else this->Selected = 0;
			break;

		case BrowserType::List:
			if (this->ListEntries.empty()) break;
			if (this->Selected < (int)this->ListEntries.size() - 1) this->Selected++;
			else this->Selected = 0;
			break;
	}
};


void BrowseData::Left(const int Amount) {
	if (this->Selected - Amount >= 0) this->Selected -= Amount;
	else this->Selected = 0;
};


void BrowseData::Right(const int Amount) {
	switch(this->Type) {
		case BrowserType::File:
			if (this->DirEntries.empty()) break;

			if (this->Selected + Amount < (int)this->DirEntries.size() - 1) this->Selected += Amount;
			else this->Selected = (int)this->DirEntries.size() - 1;
			break;

		case BrowserType::List:
			if (this->ListEntries.empty()) break;

			if (this->Selected + Amount < (int)this->ListEntries.size() - 1) this->Selected += Amount;
			else this->Selected = (int)this->ListEntries.size() - 1;
			break;
	}
};


/*
	Selected File Open Callback.

	Returns true, if it's a directory or false if not.
*/
bool BrowseData::OpenHandle() {
	switch(this->Type) {
		case BrowserType::File:
			if (this->DirEntries.empty()) return false;
			return this->DirEntries[this->Selected].IsDirectory;

		case BrowserType::List:
			return false;
	}

	return false;
};


/* Return, if you can go a directory back or not. */
bool BrowseData::CanDirBack() {
	if (this->Type == BrowserType::File) {
		char Path[PATH_MAX];
		getcwd(Path, PATH_MAX);

		/* Check for romfs, nitro, sdmc, /, sd and fat. */
		if ((strcmp(Path, "romfs:/") == 0) || (strcmp(Path, "nitro:/") == 0) || strcmp(Path, "sdmc:/") == 0 || strcmp(Path, "/") == 0 || strcmp(Path, "sd:/") == 0 || (strcmp(Path, "fat:/") == 0)) return false;
	}

	return true;
};


/* Goes a directory back. */
void BrowseData::GoDirBack() {
	if (this->Type == BrowserType::File) {
		if (!this->CanDirBack()) return;

		chdir("..");
		this->FetchDirectoryEntries();
		this->Selected = 0;
	}
};


/* Goes a directory up. */
void BrowseData::GoDirUp() {
	if (this->Type == BrowserType::File) {
		if (this->DirEntries.empty() || !this->DirEntries[this->Selected].IsDirectory) return; // Ensure it's a directory and not empty.

		const std::string Path = this->DirEntries[this->Selected].Name;
		chdir(Path.c_str());
		this->FetchDirectoryEntries();
		this->Selected = 0;
	}
};


/*
	Set the Selected Selection.

	const int Selection: The Selection to set.

	Returns true if success or false if not.
*/
bool BrowseData::SetSelection(const int Selection) {
	switch(this->Type) {
		case BrowserType::File:
			if (this->DirEntries.empty()) this->Selected = 0;

			if (Selection <= (int)this->DirEntries.size() - 1) {
				this->Selected = Selection;
				return true;
			}
			break;

		case BrowserType::List:
			if (this->ListEntries.empty()) this->Selected = 0;
			if (Selection <= (int)this->ListEntries.size() - 1) {
				this->Selected = Selection;
				return true;
			}
			break;
	}

	return false;
};


/* Returns the current working directory. */
std::string BrowseData::GetPath() const {
	char Path[PATH_MAX];
	getcwd(Path, PATH_MAX);

	return Path;
};


void BrowseData::RefreshList() {
	this->FetchDirectoryEntries(this->ExtensionList);
};