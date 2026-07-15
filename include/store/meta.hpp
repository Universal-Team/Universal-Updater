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

#ifndef _UNIVERSAL_UPDATER_META_HPP
#define _UNIVERSAL_UPDATER_META_HPP

#include <map>
#include <string>
#include <vector>

enum favoriteMarks {
	STAR = 1 << 0,
	HEART = 1 << 1,
	DIAMOND = 1 << 2,
	CLUBS = 1 << 3,
	SPADE = 1 << 4
};

class Meta {
	struct App {
		std::vector<std::string> installed;
		std::string updated;
		int marks;
	};

	// 2D map stores -> apps
	std::map<std::string, std::map<std::string, App>> apps;
	bool changesMade = false;

	static std::string emptyString;
	static std::vector<std::string> emptyVector;

public:
	Meta();
	~Meta() { this->Save(); }

	void Save();

	const std::vector<std::string> &GetInstalled(const std::string &unistoreName, const std::string &entry) const {
		if (this->apps.count(unistoreName) == 0 || this->apps.at(unistoreName).count(entry) == 0) return emptyVector;
		return this->apps.at(unistoreName).at(entry).installed;
	}

	bool GetInstalled(const std::string &unistoreName, const std::string &entry, const std::string &name) const {
		if (this->apps.count(unistoreName) == 0 || this->apps.at(unistoreName).count(entry) == 0) return false;
		for (const std::string &item : this->apps.at(unistoreName).at(entry).installed) {
			if (item == name) return true;
		}
		return false;
	}

	void SetInstalled(const std::string &unistoreName, const std::string &entry, const std::string &name) {
		if (!this->GetInstalled(unistoreName, entry, name)) {
			this->apps[unistoreName][entry].installed.push_back(name);
			this->changesMade = true;
		}
	}

	std::string GetUpdated(const std::string &unistoreName, const std::string &entry) const {
		if (this->apps.count(unistoreName) == 0 || this->apps.at(unistoreName).count(entry) == 0) return emptyString;
		return this->apps.at(unistoreName).at(entry).updated;
	}

	void SetUpdated(const std::string &unistoreName, const std::string &entry, const std::string &updated) {
		this->apps[unistoreName][entry].updated = updated;
		this->changesMade = true;
	}

	int GetMarks(const std::string &unistoreName, const std::string &entry) const {
		if (this->apps.count(unistoreName) == 0 || this->apps.at(unistoreName).count(entry) == 0) return 0;
		return this->apps.at(unistoreName).at(entry).marks;
	}

	void SetMarks(const std::string &unistoreName, const std::string &entry, int marks) {
		this->apps[unistoreName][entry].marks = marks;
		this->changesMade = true;
	}

	bool UpdateAvailable(const std::string &unistoreName, const std::string &entry, const std::string &updated) const;
	void RemoveInstalled(const std::string &unistoreName, const std::string &entry, const std::string &name);
};

#endif
