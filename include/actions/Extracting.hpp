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

#ifndef _UNIVERSAL_UPDATER_EXTRACTING_HPP
#define _UNIVERSAL_UPDATER_EXTRACTING_HPP

#include "Action.hpp"
#include <string>


class Extracting : public Action {
public:
	enum class Error : uint8_t { Good = 0, Archive, Alloc, Find, ReadFile, OpenFile, WriteFile, OutOfSpace };

	Extracting(const std::string &ArchivePath, const std::string &WantedFile, const std::string &OutputPath)
		: ArchivePath(ArchivePath), WantedFile(WantedFile), OutputPath(OutputPath) { };

	void Handler() override;

	/* Some returns. */
	std::pair<int, int> Files() const override { return { this->FilesExtracted, this->FileCount }; };
	std::pair<uint32_t, uint32_t> Progress() const override { return { this->ExtractOffs, this->ExtractSize }; };
	std::string CurrentFile() const override { return this->CFile; };
	uint8_t State() const override { return (uint8_t)this->CurState; };
	Action::ActionType Type() const override { return Action::ActionType::Extracting; };
	bool IsDone() const override { return this->Done; };
private:
	int FilesExtracted = 0, FileCount = 0; // The amount of files that are extracted, the amount of files that need to be extracted.
	uint32_t ExtractOffs = 0, ExtractSize = 0; // Current Extract offset, Size to extract completely.
	std::string CFile = ""; // The current file which is being extracted.
	Error CurState = Error::Good; // The current state of the operation.
	bool Done = false; // Is the operation already done?

	std::string ArchivePath = "", WantedFile = "", OutputPath = "";
	void FetchSize(); // Fetch the File Count and the total extract size.
};

#endif