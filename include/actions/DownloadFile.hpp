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

#ifndef _UNIVERSAL_UPDATER_DOWNLOAD_FILE_HPP
#define _UNIVERSAL_UPDATER_DOWNLOAD_FILE_HPP

#include "Action.hpp"
#include "DownloadUtils.hpp"


class DownloadFile : public Action {
public:
	enum class Error : uint8_t { Good = 0, OutOfSpace };

	DownloadFile(const std::string &URL, const std::string &Path)
		: URL(URL), Path(Path) { };

	void Handler() override;

	/* Some returns. */
	std::pair<int, int> Files() const override { return { 0, 0 }; };
	std::pair<uint32_t, uint32_t> Progress() const override { return { this->CurProg, this->TotalProg }; };
	std::string CurrentFile() const override { return ""; };
	uint8_t State() const override { return (uint8_t)this->CurState; };
	Action::ActionType Type() const override { return Action::ActionType::DownloadFile; };
	bool IsDone() const override { return this->Done; };
	
	void Cancel() override { };
private:
	uint32_t CurProg = 0, TotalProg = 0;
	Error CurState = Error::Good;
	bool Done = false;
	std::string URL = "", Path = "";
};

#endif