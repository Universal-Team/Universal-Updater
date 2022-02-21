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

#include "storeEntry.hpp"

/*
	Fetch informations on constructor.

	const std::unique_ptr<Store> &store: Const Reference to the store class.
	const std::unique_ptr<Meta> &meta: Const Reference to the meta class.
	int index: Index of the entry.
*/
StoreEntry::StoreEntry(const std::unique_ptr<Store> &store, const std::unique_ptr<Meta> &meta, int index) {
	this->Title = store->GetTitleEntry(index);
	this->Author = store->GetAuthorEntry(index);

	this->Description = store->GetDescriptionEntry(index);

	this->Category = StringUtils::FetchStringsFromVector(store->GetCategoryIndex(index));
	this->Version = store->GetVersionEntry(index);
	this->Console = StringUtils::FetchStringsFromVector(store->GetConsoleEntry(index));
	this->LastUpdated = store->GetLastUpdatedEntry(index);
	this->License = store->GetLicenseEntry(index);
	this->MarkString = StringUtils::GetMarkString(meta->GetMarks(store->GetUniStoreTitle(), this->Title));

	this->Icon = store->GetIconEntry(index);
	this->SheetIndex = 0;
	this->EntryIndex = index;

	this->FullCategory = store->GetCategoryIndex(index);
	this->FullConsole = store->GetConsoleEntry(index);

	this->UpdateAvailable = meta->UpdateAvailable(store->GetUniStoreTitle(), this->Title, store->GetLastUpdatedEntry(index));
	this->Marks = meta->GetMarks(store->GetUniStoreTitle(), this->Title);

	const std::vector<std::string> entries = store->GetDownloadList(index);

	if (!entries.empty()) {
		for (int i = 0; i < (int)entries.size(); i++) {
			this->Sizes.push_back( store->GetFileSizes(index, entries[i]) );
			this->Types.push_back( store->GetFileTypes(index, entries[i]) );
		}
	}

	this->Screenshots = store->GetScreenshotList(index);
	this->ScreenshotNames = store->GetScreenshotNames(index);
	this->ReleaseNotes = store->GetReleaseNotes(index);
}