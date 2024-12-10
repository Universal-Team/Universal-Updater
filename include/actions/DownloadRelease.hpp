// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_DOWNLOAD_RELEASE_HPP
#define _UNIVERSAL_UPDATER_DOWNLOAD_RELEASE_HPP

#include "Action.hpp"
#include "DownloadUtils.hpp"


class DownloadRelease : public Action {
public:
	enum class Error : uint8_t { Good = 0, OutOfSpace };

	DownloadRelease(const std::string &Repo, const std::string &File, const std::string &Output, const bool PreRelease = false) :
		Repo(Repo), File(File), Output(Output), PreRelease(PreRelease) { };

	void Handler() override;
	void Draw() const override;

	/* Some returns. */
	uint8_t State() const override { return (uint8_t)this->CurState; };
	Action::ActionType Type() const override { return Action::ActionType::DownloadRelease; };

private:
	uint32_t CurProg = 0, TotalProg = 0;
	Error CurState = Error::Good;
	std::string Repo = "", File = "", Output = "", URL = "";
	bool PreRelease = false;
};

#endif