// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

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