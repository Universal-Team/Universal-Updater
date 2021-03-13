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

#include "extract.hpp"
#include "queueSystem.hpp"
#include "scriptUtils.hpp"
#include <archive.h>
#include <archive_entry.h>
#include <regex>

int filesExtracted = 0, extractFilesCount = 0;
std::string extractingFile = "";

/* That are our Extract Progressbar variable. */
u32 extractSize = 0, writeOffset = 0;

Result getExtractedSize(const std::string &archivePath, const std::string &wantedFile) {
	extractSize = 0, writeOffset = 0, filesExtracted = 0, extractFilesCount = 0;

	archive *a = archive_read_new();
	archive_entry *entry;

	archive_read_support_format_all(a);

	if (archive_read_open_filename(a, archivePath.c_str(), 0x4000) != ARCHIVE_OK) return EXTRACT_ERROR_OPENFILE;

	while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		int size = archive_entry_size(entry);
		if (size > 0) { // Ignore folders.
			std::smatch match;
			std::string entryName(archive_entry_pathname(entry));
			if (std::regex_search(entryName, match, std::regex(wantedFile))) {
				extractSize += size;
				extractFilesCount++;
			}
		}
	}

	archive_read_close(a);
	archive_read_free(a);
	return EXTRACT_ERROR_NONE;
}

Result extractArchive(const std::string &archivePath, const std::string &wantedFile, const std::string &outputPath) {
	archive *a = archive_read_new();
	archive_entry *entry;

	a = archive_read_new();
	archive_read_support_format_all(a);

	if (archive_read_open_filename(a, archivePath.c_str(), 0x4000) != ARCHIVE_OK) {
		archive_read_close(a);
		archive_read_free(a);
		return EXTRACT_ERROR_OPENFILE;
	}

	while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		if (archive_entry_size(entry) > 0) { // Ignore folders.
			std::smatch match;
			std::string entryName(archive_entry_pathname(entry));
			if (std::regex_search(entryName, match, std::regex(wantedFile))) {
				extractingFile = outputPath + match.suffix().str();
				filesExtracted++;

				/* make directories. */
				for (char *slashpos = strchr(extractingFile.c_str() + 1, '/'); slashpos != NULL; slashpos = strchr(slashpos + 1, '/')) {
					char bak = *(slashpos);
					*(slashpos) = '\0';

					mkdir(extractingFile.c_str(), 0777);

					*(slashpos) = bak;
				}

				uint sizeLeft = archive_entry_size(entry);

				FILE *file = fopen(extractingFile.c_str(), "wb");
				if (!file) {
					archive_read_close(a);
					archive_read_free(a);
					return EXTRACT_ERROR_WRITEFILE;
				}

				u8 *buf = new u8[0x30000];
				if (!buf) {
					fclose(file);
					archive_read_close(a);
					archive_read_free(a);
					return EXTRACT_ERROR_ALLOC;
				}

				while(sizeLeft > 0) {
					u64 toRead = std::min(0x30000u, sizeLeft);
					ssize_t size = archive_read_data(a, buf, toRead);

					/* Archive error, stop extracting. */
					if (size < 0) {
						fclose(file);
						delete[] buf;
						archive_read_close(a);
						archive_read_free(a);
						return EXTRACT_ERROR_ARCHIVE;
					}

					fwrite(buf, 1, size, file);
					sizeLeft -= size;
					writeOffset += size;
				}

				fclose(file);
				delete[] buf;

				if (QueueSystem::CancelCallback) goto exit; // Cancel Extraction.
			}
		}
	}

	exit:
		archive_read_close(a);
		archive_read_free(a);
		return EXTRACT_ERROR_NONE;
}