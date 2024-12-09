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
*   MERCHANTABILITY or FITNESS FOR Archive PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received Archive copy of the GNU General Public License
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

#include "Moving.hpp"

#include "Utils.hpp"

#include <unistd.h>


void Moving::Handler() {
	if (access(this->OldName.c_str(), F_OK) != 0) {
		this->CurState = Moving::Error::SourceNotExist;
		this->Done = true;
		return;
	}

	/* Make directories. */
	Utils::MakeDirs(this->NewName);
	

	rename(this->OldName.c_str(), this->NewName.c_str());
	this->CurState = Moving::Error::Good;
	this->Done = true;
};


void Moving::Draw() const {
	// TODO
};
