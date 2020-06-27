#include "files.h"

FS_Path getPathInfo(const char * path, FS_ArchiveID * archive) {
	*archive = ARCHIVE_SDMC;
	FS_Path filePath = {0};
	unsigned int prefixlen = 0;

	if (!strncmp(path, "sdmc:/", 6)) {
		prefixlen = 5;
	} else if (*path != '/') {
		//if the path is local (doesnt start with a slash), it needs to be appended to the working dir to be valid
		char * actualPath = NULL;
		asprintf(&actualPath, "%s%s", WORKING_DIR, path);
		filePath = fsMakePath(PATH_ASCII, actualPath);
		free(actualPath);
	}

	//if the filePath wasnt set above, set it
	if (filePath.size == 0) {
		filePath = fsMakePath(PATH_ASCII, path+prefixlen);
	}

	return filePath;
}

Result makeDirs(const char * path) {
	Result ret = 0;
	FS_ArchiveID archiveID;
	FS_Path filePath = getPathInfo(path, &archiveID);
	FS_Archive archive;

	ret = FSUSER_OpenArchive(&archive, archiveID, fsMakePath(PATH_EMPTY, ""));

	for (char * slashpos = strchr(path+1, '/'); slashpos != NULL; slashpos = strchr(slashpos+1, '/')) {
		char bak = *(slashpos);
		*(slashpos) = '\0';
		Handle dirHandle;

		ret = FSUSER_OpenDirectory(&dirHandle, archive, filePath);
		if (R_SUCCEEDED(ret))	FSDIR_Close(dirHandle);
		else	ret = FSUSER_CreateDirectory(archive, filePath, FS_ATTRIBUTE_DIRECTORY);

		*(slashpos) = bak;
	}

	FSUSER_CloseArchive(archive);

	return ret;
}

Result openFile(Handle* fileHandle, const char * path, bool write) {
	FS_ArchiveID archive;
	FS_Path filePath = getPathInfo(path, &archive);
	u32 flags = (write ? (FS_OPEN_CREATE | FS_OPEN_WRITE) : FS_OPEN_READ);

	Result ret = 0;
	ret = makeDirs(strdup(path));
	ret = FSUSER_OpenFileDirectly(fileHandle, archive, fsMakePath(PATH_EMPTY, ""), filePath, flags, 0);
	if (write)	ret = FSFILE_SetSize(*fileHandle, 0); //truncate the file to remove previous contents before writing

	return ret;
}

Result deleteFile(const char * path) {
	FS_ArchiveID archiveID;
	FS_Path filePath = getPathInfo(path, &archiveID);

	FS_Archive archive;
	Result ret = FSUSER_OpenArchive(&archive, archiveID, fsMakePath(PATH_EMPTY, ""));
	if (R_FAILED(ret)) return ret;
	ret = FSUSER_DeleteFile(archive, filePath);
	FSUSER_CloseArchive(archive);

	return ret;
}

Result removeDir(const char *path) {
	FS_ArchiveID archiveID;
	FS_Path filePath = getPathInfo(path, &archiveID);
	FS_Archive archive;

	Result ret = FSUSER_OpenArchive(&archive, archiveID, fsMakePath(PATH_EMPTY, ""));
	if (R_FAILED(ret)) return ret;
	ret = FSUSER_DeleteDirectory(archive, filePath);
	FSUSER_CloseArchive(archive);

	return ret;
}

Result removeDirRecursive(const char *path) {
	FS_ArchiveID archiveID;
	FS_Path filePath = getPathInfo(path, &archiveID);
	FS_Archive archive;

	Result ret = FSUSER_OpenArchive(&archive, archiveID, fsMakePath(PATH_EMPTY, ""));
	if (R_FAILED(ret)) return ret;
	ret = FSUSER_DeleteDirectoryRecursively(archive, filePath);
	FSUSER_CloseArchive(archive);
	
	return ret;
}