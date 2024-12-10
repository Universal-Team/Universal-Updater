// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "Copying.hpp"

#include "Utils.hpp"

#include <dirent.h>
#include <unistd.h>


void Copying::Handler() { this->FileCopy(this->Source, this->Dest); };


void Copying::Draw() const {
	// TODO
};


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
