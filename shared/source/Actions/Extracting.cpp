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

#include "Extracting.hpp"
#include <archive.hpp>
#include <archive_entry.hpp>
#include <regex>


void Extracting::FetchSize() {
	/* Reset all of them here. */
	this->ExtractOffs = 0, this->ExtractSize = 0, this->FilesExtracted = 0, this->FileCount = 0;

	archive *Archive = archive_read_new();
	archive_entry *Entry;

	archive_read_support_format_all(Archive);

	if (archive_read_open_filename(Archive, this->ArchivePath.c_str(), 0x4000) != ARCHIVE_OK) {
		this->CurState = Extracting::Error::OpenFile;
		archive_read_free(Archive);
		return;
	}

	while(archive_read_next_header(Archive, &Entry) == ARCHIVE_OK) {
		int Size = archive_entry_size(Entry);
		std::smatch Match;
		std::string EntryName(archive_entry_pathname(Entry));

		if (std::regex_search(EntryName, Match, std::regex(this->WantedFile))) {
			this->ExtractSize += Size;
			this->FileCount++;
		}
	}

	archive_read_close(Archive);
	archive_read_free(Archive);
	return;
};


void Extracting::Handler() {
	this->FetchSize(); // Fetch size.
	if (this->CurState != Extracting::Error::Good) {
		this->Done = true;
		return;
	}

	archive *Archive = archive_read_new();
	archive_entry *Entry;

	archive_read_support_format_all(Archive);

	if (archive_read_open_filename(Archive, this->ArchivePath.c_str(), 0x4000) != ARCHIVE_OK) {
		archive_read_free(Archive);
		this->CurState = Extracting::Error::OpenFile;
		this->Done = true;
		return;
	}

	while(archive_read_next_header(Archive, &Entry) == ARCHIVE_OK) {
		std::smatch Match;
		std::string EntryName(archive_entry_pathname(Entry));
		if (std::regex_search(EntryName, Match, std::regex(this->WantedFile))) {
			this->CFile = this->OutputPath + Match.suffix().str();
			this->FilesExtracted++;

			/* Make directories. */
			for (char *Slashpos = strchr(this->CFile.c_str() + 1, '/'); Slashpos != NULL; Slashpos = strchr(Slashpos + 1, '/')) {
				char Bak = *(Slashpos);
				*(Slashpos) = '\0';

				mkdir(this->CFile.c_str(), 0x777);

				*(Slashpos) = Bak;
			}

			/* If directory then mkdir it and skip extraction. */
			if (S_ISDIR(archive_entry_mode(Entry))) {
				mkdir(this->CFile.c_str(), 0x777);
				continue;
			}

			uint64_t SizeLeft = archive_entry_size(Entry);

			FILE *File = fopen(this->CFile.c_str(), "wb");
			if (!File) {
				archive_read_close(Archive);
				archive_read_free(Archive);
				this->CurState = Extracting::Error::WriteFile;
				this->Done = true;
				goto Exit;
			}

			uint8_t *Buffer = new uint8_t[0x30000];
			if (!Buffer) {
				fclose(File);
				archive_read_close(Archive);
				archive_read_free(Archive);
				this->CurState = Extracting::Error::Alloc;
				this->Done = true;
				goto Exit;
			}

			while(SizeLeft > 0) {
				const uint64_t ToRead = std::min<uint64_t>(0x30000u, SizeLeft);
				const ssize_t Size = archive_read_data(Archive, Buffer, ToRead);

				/* Archive error, stop extracting. */
				if (Size < 0) {
					fclose(File);
					delete[] Buffer;
					archive_read_close(Archive);
					archive_read_free(Archive);
					this->CurState = Extracting::Error::Archive;
					this->Done = true;
					return;
				}

				const ssize_t Written = fwrite(Buffer, 1, Size, File);

				/* Failed to write, likely out of space. */
				if (Written != Size) {
					fclose(File);
					delete[] Buffer;
					archive_read_close(Archive);
					archive_read_free(Archive);
					this->CurState = Extracting::Error::WriteFile;
					this->Done = true;
					goto Exit;
				}

				SizeLeft -= Size;
				this->ExtractOffs += Size;
			}

			fclose(File);
			delete[] Buffer;
		}
	}

	goto Exit;

	Exit:
		archive_read_close(Archive);
		archive_read_free(Archive);
		this->Done = true;
};