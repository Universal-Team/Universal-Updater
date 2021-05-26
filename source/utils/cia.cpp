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

#include "cia.hpp"
#include "files.hpp"

Result Title::Launch(u64 titleId, FS_MediaType mediaType) {
	Result ret = 0;
	u8 param[0x300];
	u8 hmac[0x20];

	if (R_FAILED(ret = APT_PrepareToDoApplicationJump(0, titleId, mediaType))) {
		printf("Error In:\nAPT_PrepareToDoApplicationJump");
		return ret;
	}

	if (R_FAILED(ret = APT_DoApplicationJump(param, sizeof(param), hmac))) {
		printf("Error In:\nAPT_DoApplicationJump");
		return ret;
	}

	return 0;
}

Result Title::DeletePrevious(u64 titleid, FS_MediaType media) {
	Result ret = 0;
	u32 titles_amount = 0;

	ret = AM_GetTitleCount(media, &titles_amount);
	if (R_FAILED(ret)) {
		printf("Error in:\nAM_GetTitleCount\n");
		return ret;
	}

	u32 read_titles = 0;
	u64 *titleIDs = (u64 *)malloc(titles_amount * sizeof(u64));

	ret = AM_GetTitleList(&read_titles, media, titles_amount, titleIDs);
	if (R_FAILED(ret)) {
		free(titleIDs);
		printf("Error in:\nAM_GetTitleList\n");
		return ret;
	}

	for (u32 i = 0; i < read_titles; i++) {
		if (titleIDs[i] == titleid) {
			ret = AM_DeleteAppTitle(media, titleid);
			break;
		}
	}

	free(titleIDs);

	if (R_FAILED(ret)) {
		printf("Error in:\nAM_DeleteAppTitle\n");
		return ret;
	}

	return 0;
}

static FS_MediaType getTitleDestination(u64 titleId) {
	u16 platform = (u16) ((titleId >> 48) & 0xFFFF);
	u16 category = (u16) ((titleId >> 32) & 0xFFFF);
	u8 variation = (u8) (titleId & 0xFF);

	//     DSiWare                3DS                    DSiWare, System, DLP         Application           System Title
	return platform == 0x0003 || (platform == 0x0004 && ((category & 0x8011) != 0 || (category == 0x0000 && variation == 0x02))) ? MEDIATYPE_NAND : MEDIATYPE_SD;
}

u32 installSize = 0, installOffset = 0;

Result Title::Install(const char *ciaPath, bool updatingSelf) {
	u32 bytes_read = 0, bytes_written;
	installSize = 0, installOffset = 0; u64 size = 0;
	Handle ciaHandle, fileHandle;
	AM_TitleEntry info;
	Result ret = 0;
	FS_MediaType media = MEDIATYPE_SD;

	ret = openFile(&fileHandle, ciaPath, false);
	if (R_FAILED(ret)) {
		printf("Error in:\nopenFile\n");
		return ret;
	}

	ret = AM_GetCiaFileInfo(media, &info, fileHandle);
	if (R_FAILED(ret)) {
		printf("Error in:\nAM_GetCiaFileInfo\n");
		return ret;
	}

	media = getTitleDestination(info.titleID);

	if (!updatingSelf) {
		ret = Title::DeletePrevious(info.titleID, media);
		if (R_FAILED(ret)) return ret;
	}

	ret = FSFILE_GetSize(fileHandle, &size);
	if (R_FAILED(ret)) {
		printf("Error in:\nFSFILE_GetSize\n");
		FSFILE_Close(fileHandle);
		return ret;
	}

	if (getAvailableSpace() >= size) {
		ret = AM_StartCiaInstall(media, &ciaHandle);
		if (R_FAILED(ret)) {
			printf("Error in:\nAM_StartCiaInstall\n");
			FSFILE_Close(fileHandle);
			return ret;
		}

		u32 toRead = 0x200000;
		u8 *buf = new u8[toRead];

		if (!buf) {
			FSFILE_Close(fileHandle);
			return -1;
		}

		installSize = size;
		do {
			FSFILE_Read(fileHandle, &bytes_read, installOffset, buf, toRead);
			FSFILE_Write(ciaHandle, &bytes_written, installOffset, buf, toRead, FS_WRITE_FLUSH);
			installOffset += bytes_read;
		} while(installOffset < installSize);
		delete[] buf;

		ret = AM_FinishCiaInstall(ciaHandle);
		if (R_FAILED(ret)) {
			printf("Error in:\nAM_FinishCiaInstall\n");
			FSFILE_Close(fileHandle);
			return ret;
		}
	}

	ret = FSFILE_Close(fileHandle);
	if (R_FAILED(ret)) {
		printf("Error in:\nFSFILE_Close\n");
		return ret;
	}

	if (updatingSelf) {
		if (R_FAILED(ret = Title::Launch(info.titleID, MEDIATYPE_SD))) return ret;
	}

	return 0;
}