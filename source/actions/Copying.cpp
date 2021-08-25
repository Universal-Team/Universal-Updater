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

#include "Copying.hpp"
#include "Utils.hpp"
#include <dirent.h>
#include <unistd.h>


void Copying::Handler() { this->FileCopy(this->Source, this->Dest); };


void Copying::FileCopy(const std::string &Source, const std::string &Dest) {
	DIR *IsDir = opendir(Source.c_str());

	if (IsDir != NULL) {
		closedir(IsDir);

		/* Source path is a directory. */
		std::vector<BrowseData::DirEntry> DirContents;
		std::unique_ptr<BrowseData> BData = std::make_unique<BrowseData>(Source);
		DirContents = BData->GetDirEntries();
		mkdir(this->Dest.c_str(), 0777);

		for(int Idx = 1; Idx < ((int)DirContents.size()); Idx++) this->DirCopy(DirContents[Idx], Source, Dest);

		chdir(Dest.c_str());

		this->CurState = Copying::Error::Good;
		this->Done = true;
		return;

	} else {
		closedir(IsDir);

		/* Source path is a file. */
		FILE *SourceFile = fopen(Source.c_str(), "rb");
		this->CopySize = 0, this->CopyOffs = 0;

		if (SourceFile) {
			fseek(SourceFile, 0, SEEK_END);
			this->CopySize = ftell(SourceFile); // Get source file's size.
			fseek(SourceFile, 0, SEEK_SET);

		} else {
			this->CurState = Copying::Error::SourceNotExist;
			this->Done = true;
			return;
		}

		
		if (Utils::AvailableSpace() < this->CopySize) {
			fclose(SourceFile);
			this->CurState = Copying::Error::OutOfSpace;
			this->Done = true;
			return;
		}
		
		/* Make directories. */
		Utils::MakeDirs(Dest);

		FILE *DestFile = fopen(Dest.c_str(), "wb");
		if (!DestFile) {
			fclose(SourceFile);

			this->CurState = Copying::Error::DestNotExist;
			this->Done = true;
			return;
		}

		while(1) {
			/* Copy file to destination path. */
			const int Read = fread(this->CopyBuf, sizeof(uint32_t), this->CopyBufSize, SourceFile);
			const int Written = fwrite(this->CopyBuf, sizeof(uint32_t), Read, DestFile);

			if (Written != Read) {
				fclose(SourceFile);
				fclose(DestFile);

				this->CurState = Copying::Error::WrittenNotRead;
				this->Done = true;
				return;

			}

			this->CopyOffs += this->CopyBufSize * sizeof(uint32_t);

			if (this->CopyOffs > this->CopySize) {
				fclose(SourceFile);
				fclose(DestFile);

				this->CurState = Copying::Error::Good;
				this->Done = true;
				return;
			}
		}

		this->CurState = Copying::Error::Unknown;
		this->Done = true;
	}
};


void Copying::DirCopy(const BrowseData::DirEntry &Entry, const std::string &Source, const std::string &Dest) {
	std::vector<BrowseData::DirEntry> DirContents;

	std::unique_ptr<BrowseData> BData = std::make_unique<BrowseData>((Entry.IsDirectory ? (Source + "/" + Entry.Name) : Source));
	DirContents = BData->GetDirEntries();

	if (((int)DirContents.size()) == 1) mkdir((Dest.c_str() + ("/" + Entry.Name)).c_str(), 0777);
	if (((int)DirContents.size()) != 1) this->FileCopy(Source + "/" + Entry.Name, Dest + "/" + Entry.Name);
};


/* TODO: Come up with a good way. */
void Copying::Cancel() {

};