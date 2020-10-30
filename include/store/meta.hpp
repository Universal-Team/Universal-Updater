/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 Universal-Team
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

#ifndef _UNIVERSAL_UPDATER_META_HPP
#define _UNIVERSAL_UPDATER_META_HPP

#include "json.hpp"
#include <string>

enum favoriteMarks {
	STAR = 1 << 0,
	HEART = 1 << 1,
	DIAMOND = 1 << 2,
	CLUBS = 1 << 3,
	SPADE = 1 << 4
};

class Meta {
public:
	Meta();
	~Meta() { this->SaveCall(); };

	std::string GetUpdated(std::string unistoreName, std::string entry) const;
	int GetMarks(std::string unistoreName, std::string entry) const;
	bool UpdateAvailable(std::string unistoreName, std::string entry, std::string updated) const;

	void SetUpdated(std::string unistoreName, std::string entry, std::string updated) {
		this->metadataJson[unistoreName][entry]["updated"] = updated;
	};

	void SetMarks(std::string unistoreName, std::string entry, int marks) {
		this->metadataJson[unistoreName][entry]["marks"] = marks;
	};

	void ImportMetadata();
	void SaveCall();
private:
	nlohmann::json metadataJson = nullptr;
};

#endif