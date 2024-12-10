// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_COPYING_HPP
#define _UNIVERSAL_UPDATER_COPYING_HPP

#include "Action.hpp"
#include "BrowseData.hpp"
#include <memory>
#include <string>


/*
	Handles Copying of Files.
*/
class Copying : public Action {
public:
	enum class Error : uint8_t { Good = 0, SourceNotExist, DestNotExist, WrittenNotRead, Unknown, OutOfSpace };

	Copying(const std::string &Source, const std::string &Dest)
		: Source(Source), Dest(Dest) { };

	void Handler() override;
	void Draw() const override;

	/* Some returns. */
	uint8_t State() const override { return (uint8_t)this->CurState; };
	Action::ActionType Type() const override { return Action::ActionType::Copying; };
	
private:
	uint32_t CopyOffs = 0, CopySize = 0;
	Error CurState = Error::Good; // The current state of the operation.
	std::string Source = "", Dest = "";

	/* Some Helpers. */
	static constexpr int CopyBufSize = 0x8000;
	uint32_t CopyBuf[CopyBufSize];
	
	void FileCopy(const std::string &Source, const std::string &Dest);
	void DirCopy(const BrowseData::DirEntry &Entry, const std::string &Source, const std::string &Dest);
};

#endif