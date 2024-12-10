// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

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
	void Draw() const override;

	/* Some returns. */
	uint8_t State() const override { return (uint8_t)this->CurState; };
	Action::ActionType Type() const override { return Action::ActionType::Extracting; };

private:
	/* TODO: Maybe sort out what is really needed or so. That looks like a mess. */
	int FilesExtracted = 0, FileCount = 0; // The amount of files that are extracted, the amount of files that need to be extracted.
	uint32_t ExtractOffs = 0, ExtractSize = 0; // Current Extract offset, Size to extract completely.
	std::string CFile = ""; // The current file which is being extracted.
	Error CurState = Error::Good; // The current state of the operation.
	std::string ArchivePath = "", WantedFile = "", OutputPath = "";
	void FetchSize(); // Fetch the File Count and the total extract size.
};

#endif