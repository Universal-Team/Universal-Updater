#include "cia.h"

bool updatingSelf = false;

static Result CIA_LaunchTitle(u64 titleId, FS_MediaType mediaType) {
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

Result deletePrevious(u64 titleid, FS_MediaType media)
{
	Result ret = 0;

	u32 titles_amount = 0;
	ret = AM_GetTitleCount(media, &titles_amount);
	if (R_FAILED(ret)) {
		printf("Error in:\nAM_GetTitleCount\n");
		return ret;
	}

	u32 read_titles = 0;
	u64 * titleIDs = malloc(titles_amount * sizeof(u64));
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

FS_MediaType getTitleDestination(u64 titleId) {
	u16 platform = (u16) ((titleId >> 48) & 0xFFFF);
	u16 category = (u16) ((titleId >> 32) & 0xFFFF);
	u8 variation = (u8) (titleId & 0xFF);

	//     DSiWare                3DS                    DSiWare, System, DLP         Application           System Title
	return platform == 0x0003 || (platform == 0x0004 && ((category & 0x8011) != 0 || (category == 0x0000 && variation == 0x02))) ? MEDIATYPE_NAND : MEDIATYPE_SD;
}


Result installCia(const char * ciaPath)
{
	u64 size = 0;
	u32 bytes;
	Handle ciaHandle;
	Handle fileHandle;
	AM_TitleEntry info;
	Result ret = 0;

	ret = amInit();
	if (R_FAILED(ret)) {
		printf("Error in:\namInit();\n");
		return ret;
	}

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
	if (info.titleID == 0x0004000004391700) {
		updatingSelf = true;
	}

	if (!updatingSelf) {
		ret = deletePrevious(info.titleID, media);
		if (R_FAILED(ret))
			return ret;
	}

	ret = FSFILE_GetSize(fileHandle, &size);
	if (R_FAILED(ret)) {
		printf("Error in:\nFSFILE_GetSize\n");
		return ret;
	}
	ret = AM_StartCiaInstall(media, &ciaHandle);
	if (R_FAILED(ret)) {
		printf("Error in:\nAM_StartCiaInstall\n");
		return ret;
	}

	u32 toRead = 0x20000;
	u8 * cia_buffer = memalign(0x1000, toRead);
	for (u64 startSize = size; size != 0; size -= toRead) {
		if (size < toRead) toRead = size;
		FSFILE_Read(fileHandle, &bytes, startSize-size, cia_buffer, toRead);
		FSFILE_Write(ciaHandle, &bytes, startSize-size, cia_buffer, toRead, 0);
	}
	free(cia_buffer);

	ret = AM_FinishCiaInstall(ciaHandle);
	if (R_FAILED(ret)) {
		printf("Error in:\nAM_FinishCiaInstall\n");
		return ret;
	}
	ret = FSFILE_Close(fileHandle);
	if (R_FAILED(ret)) {
		printf("Error in:\nFSFILE_Close\n");
		return ret;
	}

	if (updatingSelf) {
		if (R_FAILED(ret = CIA_LaunchTitle(info.titleID, MEDIATYPE_SD)))
			return ret;
	}
	amExit();
	return 0;
}