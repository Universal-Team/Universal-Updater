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

#include "common.hpp"
#include "meta.hpp"

#include <unistd.h>

/*
	The Constructor of the Meta.

	Includes MetaData file creation, if non existent.
*/
Meta::Meta() {
	if (access("sdmc:/3ds/Universal-Updater/MetaData.json", F_OK) != 0) {
		FILE *temp = fopen("sdmc:/3ds/Universal-Updater/MetaData.json", "w");
		char tmp[2] = { '{', '}' };
		fwrite(tmp, sizeof(tmp), 1, temp);
		fclose(temp);
	}

	FILE *temp = fopen("sdmc:/3ds/Universal-Updater/MetaData.json", "rt");
	this->metadataJson = nlohmann::json::parse(temp, nullptr, false);
	fclose(temp);
}

/*
	Get Last Updated.

	std::string unistoreName: The UniStore name.
	std::string entry: The Entry name.
*/
std::string Meta::GetUpdated(std::string unistoreName, std::string entry) const {
	if (!this->metadataJson.contains(unistoreName)) return "";

	if (!this->metadataJson[unistoreName].contains(entry)) return "";

	if (!this->metadataJson[unistoreName][entry].contains("updated")) return "";

	return this->metadataJson[unistoreName][entry]["updated"];
}

/*
	Get the marks.

	std::string unistoreName: The UniStore name.
	std::string entry: The Entry name.
*/
int Meta::GetMarks(std::string unistoreName, std::string entry) const {
	int temp = 0;

	if (!this->metadataJson.contains(unistoreName)) return temp;

	if (!this->metadataJson[unistoreName].contains(entry)) return temp;

	if (!this->metadataJson[unistoreName][entry].contains("marks")) return temp;

	return this->metadataJson[unistoreName][entry]["marks"];
}

/*
	Return, if update available.

	std::string unistoreName: The UniStore name.
	std::string entry: The Entry name.
	std::string updated: Compare for the update.
*/
bool Meta::UpdateAvailable(std::string unistoreName, std::string entry, std::string updated) const {
	if (this->GetUpdated(unistoreName, entry) != "" && updated != "") {
		return strcasecmp(updated.c_str(), this->GetUpdated(unistoreName, entry).c_str()) > 0;
	}

	return false;
}

/*
	The save call.

	Write to file.. called on destructor.
*/
void Meta::SaveCall() {
	FILE *file = fopen("sdmc:/3ds/Universal-Updater/MetaData.json", "wb");
	const std::string dump = this->metadataJson.dump(1, '\t');
	fwrite(dump.c_str(), 1, dump.size(), file);
	fclose(file);
}