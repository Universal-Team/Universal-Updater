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

#include "argumentParser.hpp"
#include "common.hpp"
#include "scriptUtils.hpp"
#include <unistd.h>

/*
	The constructor of the Argument Parser.

	const std::string &file: Const Reference to the file.
	const std::string &entry: Const Reference to the Entry Title name.
	int dlIndex: The Download index.
*/
ArgumentParser::ArgumentParser(const std::string &file, const std::string &entry, int dlIndex) {
	if (dlIndex != -1 && file != "") {
		this->file = file;
		this->entry = entry;
		this->dlIndex = dlIndex;

		this->Load();
	}
}

/*
	Prepare UniStore and get valid state.
*/
void ArgumentParser::Load() {
	if (access((std::string(_STORE_PATH) + this->file).c_str(), F_OK) != 0) return;

	this->store = std::make_unique<Store>(_STORE_PATH + this->file, this->file, true);
	if (!this->store->GetValid()) return;

	for (int i = 0; i < this->store->GetStoreSize(); i++) {
		if (this->store->GetTitleEntry(i) == this->entry) {
			this->entryIndex = i;
			const std::vector<std::string> dlList = this->store->GetDownloadList(this->entryIndex);

			if (dlList.empty()) return;

			if ((int)dlList.size() >= this->dlIndex) {
				this->executeEntry = dlList[this->dlIndex];
				this->isValid = true;
				return;
			}
		}
	}
}

/*
	Execute the Argument's entry, if valid.
*/
void ArgumentParser::Execute() {
	if (this->isValid) {
		if (Msg::promptMsg(Lang::get("EXECUTE_ENTRY") + "\n\n" + this->executeEntry)) {
			ScriptUtils::runFunctions(this->store->GetJson(), this->entryIndex, this->executeEntry);
		}
	}
}