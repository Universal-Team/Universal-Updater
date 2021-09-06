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

#include "DownloadRelease.hpp"
#include "JSON.hpp"
#include "Utils.hpp"
#include <string.h>


/*
	TODO: Proper handling or so.
*/
void DownloadRelease::Handler() {
	const std::string APIURL = "https://api.github.com/repos/" + this->Repo + (PreRelease ? "/releases" : "/releases/latest");
	char *Buffer = new char[1 << 20];

	int Res = DownloadUtils::DownloadToMemory(APIURL.c_str(), Buffer, 1 << 20);

	if (Res > 0) { // If success.
		if (nlohmann::json::accept(Buffer)) {
			nlohmann::json ParsedAPI = nlohmann::json::parse(Buffer);

			if (ParsedAPI.size() == 0) {
				this->Done = true;
				delete[] Buffer;
				return; // All were prereleases and those are being ignored.
			}

			if (Res != -2) {
				if (this->PreRelease) ParsedAPI = ParsedAPI[0];

				if (ParsedAPI["assets"].is_array()) {
					for (auto JSONAsset : ParsedAPI["assets"]) {
						if (JSONAsset.is_object() && JSONAsset["name"].is_string() && JSONAsset["browser_download_url"].is_string()) {
							std::string AssetName = JSONAsset["name"];

							if (Utils::MatchPattern(this->File, AssetName)) {
								this->URL = JSONAsset["browser_download_url"];
								break;
							}
						}
					}
				}
			}

		} else {
			this->Done = true;
			delete[] Buffer;
			return; // Bad JSON.
		}

	} else {
		this->Done = true;
		delete[] Buffer;
		return; // Download failed.
	}

	delete[] Buffer;
	/* If good, we can download. */
	if (!this->URL.empty() && Res > 0) {
		Utils::MakeDirs(this->Output);

		Res = DownloadUtils::DownloadToFile(this->URL.c_str(), this->Output.c_str());
	}

	this->Done = true;
};


void DownloadRelease::Draw() const {
	// TODO
};
