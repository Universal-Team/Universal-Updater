// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_DOWNLOAD_FILE_HPP
#define _UNIVERSAL_UPDATER_DOWNLOAD_FILE_HPP

#include "Action.hpp"


class DownloadFile : public Action {
public:
	enum class Error : uint8_t { Good = 0, OutOfSpace };

	DownloadFile(const std::string &URL, const std::string &Path)
		: URL(URL), Path(Path) { };

	void Handler() override;
	void Draw() const override;

	/* Some returns. */
	uint8_t State() const override { return (uint8_t)this->CurState; };
	Action::ActionType Type() const override { return Action::ActionType::DownloadFile; };
private:
	uint32_t CurProg = 0, TotalProg = 0;
	Error CurState = Error::Good;
	std::string URL = "", Path = "";
};

#endif