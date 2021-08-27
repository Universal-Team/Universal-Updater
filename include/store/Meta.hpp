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

#include "JSON.hpp"
#include <string>
#include <vector>


class Meta {
public:
	enum FavoriteMarks {
		STAR = 1 << 0,
		HEART = 1 << 1,
		DIAMOND = 1 << 2,
		CLUBS = 1 << 3,
		SPADE = 1 << 4
	};

	Meta();
	~Meta() { this->SaveCall(); };

	std::string GetUpdated(const std::string &UniStoreName, const std::string &Entry) const;
	int GetMarks(const std::string &UniStoreName, const std::string &Entry) const;
	bool UpdateAvailable(const std::string &UniStoreName, const std::string &Entry, const std::string &Updated) const;
	std::vector<std::string> GetInstalled(const std::string &UniStoreName, const std::string &Entry) const;

	void SetUpdated(const std::string &UniStoreName, const std::string &Entry, const std::string &Updated) {
		this->MetaDataJSON[UniStoreName][Entry]["updated"] = Updated;
	};

	void SetMarks(const std::string &UniStoreName, const std::string &Entry, const int Marks) {
		this->MetaDataJSON[UniStoreName][Entry]["marks"] = Marks;
	};


	void SetInstalled(const std::string &UniStoreName, const std::string &Entry, const std::string &Name) {
		const std::vector<std::string> Installs = this->GetInstalled(UniStoreName, Entry);
		bool Write = true;

		if (!Installs.empty()) {
			Write = !Installs.empty();

			for (size_t Idx = 0; Idx < Installs.size(); Idx++) {
				if (Installs[Idx] == Name) {
					Write = false;
					break;
				}
			}
		}

		if (Write) this->MetaDataJSON[UniStoreName][Entry]["installed"] += Name;
	};

	/* Remove installed state from a download list entry. */
	void RemoveInstalled(const std::string &UniStoreName, const std::string &Entry, const std::string &Name) {
		const std::vector<std::string> Installs = this->GetInstalled(UniStoreName, Entry);
		if (Installs.empty()) return;

		for (size_t Idx = 0; Idx < Installs.size(); Idx++) {
			if (Installs[Idx] == Name) {
				this->MetaDataJSON[UniStoreName][Entry]["installed"].erase(Idx);
				break;
			}
		}

		if (this->MetaDataJSON[UniStoreName][Entry]["installed"].empty() && this->MetaDataJSON[UniStoreName][Entry].contains("updated")) this->MetaDataJSON[UniStoreName][Entry].erase("updated");
	};

	void ImportMetadata();
	void SaveCall();
private:
	nlohmann::json MetaDataJSON = nullptr;
};

#endif