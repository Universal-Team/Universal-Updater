#include "gui.hpp"

#include "screens/screenCommon.hpp"

#include "utils/common.hpp"
#include "utils/config.hpp"
#include "utils/fileBrowse.h"
#include "utils/structs.hpp"

#include <3ds.h>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include <algorithm>
#include <functional>
#include <array>
#include <iostream>

int file_count = 0;

extern bool continueNdsScan;
extern uint selectedFile;
extern int keyRepeatDelay;
extern bool dirChanged;
std::vector<DirEntry> dirContents;

extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern touchPosition touch;

std::vector<Structs::ButtonPos> buttonPositions = {
	{295, 0, 25, 25, -1}, // Arrow Up.
	{295, 215, 25, 25, -1}, // Arrow Down.
	{15, 220, 50, 15, -1}, // Open.
	{80, 220, 50, 15, -1}, // Select.
	{145, 220, 50, 15, -1}, // Refresh.
	{210, 220, 50, 15, -1}, // Back.
	{0, 0, 25, 25, -1}, // ViewMode Change.
};

/**
 * Get the title ID.
 * @param ndsFile DS ROM image.
 * @param buf Output buffer for title ID. (Must be at least 4 characters.)
 * @return 0 on success; non-zero on error.
 */
int grabTID(FILE *ndsFile, char *buf) {
	fseek(ndsFile, offsetof(sNDSHeadertitlecodeonly, gameCode), SEEK_SET);
	size_t read = fread(buf, 1, 4, ndsFile);
	return !(read == 4);
}

void findNdsFiles(vector<DirEntry>& dirContents) {
	struct stat st;
	DIR *pdir = opendir(".");

	if (pdir == NULL) {
		DisplayMsg("Unable to open the directory.");
		for(int i=0;i<120;i++)
			gspWaitForVBlank();
	} else {
		while (continueNdsScan)
		{
			DirEntry dirEntry;

			struct dirent* pent = readdir(pdir);
			if (pent == NULL) break;

			stat(pent->d_name, &st);
			dirEntry.name = pent->d_name;
			char scanningMessage[512];
			snprintf(scanningMessage, sizeof(scanningMessage), "Scanning SD card for DS roms...\n\n(Press B to cancel)\n\n\n\n\n\n\n\n\n%s", dirEntry.name.c_str());
			DisplayMsg(scanningMessage);
			dirEntry.isDirectory = (st.st_mode & S_IFDIR) ? true : false;
				if(!(dirEntry.isDirectory) && dirEntry.name.length() >= 3) {
					if (strcasecmp(dirEntry.name.substr(dirEntry.name.length()-3, 3).c_str(), "nds") == 0) {
						// Get game's TID
						FILE *f_nds_file = fopen(dirEntry.name.c_str(), "rb");
						// char game_TID[5];
						grabTID(f_nds_file, dirEntry.tid);
						dirEntry.tid[4] = 0;
						fclose(f_nds_file);

						// dirEntry.tid = game_TID;

						dirContents.push_back(dirEntry);
						file_count++;
					}
				} else if (dirEntry.isDirectory
				&& dirEntry.name.compare(".") != 0
				&& dirEntry.name.compare("_nds") != 0
				&& dirEntry.name.compare("3ds") != 0
				&& dirEntry.name.compare("DCIM") != 0
				&& dirEntry.name.compare("gm9") != 0
				&& dirEntry.name.compare("luma") != 0
				&& dirEntry.name.compare("Nintendo 3DS") != 0
				&& dirEntry.name.compare("private") != 0
				&& dirEntry.name.compare("retroarch") != 0) {
					chdir(dirEntry.name.c_str());
					findNdsFiles(dirContents);
					chdir("..");
				}
		}
		closedir(pdir);
	}
}

off_t getFileSize(const char *fileName)
{
	FILE* fp = fopen(fileName, "rb");
	off_t fsize = 0;
	if (fp) {
		fseek(fp, 0, SEEK_END);
		fsize = ftell(fp);			// Get source file's size
		fseek(fp, 0, SEEK_SET);
	}
	fclose(fp);

	return fsize;
}

bool nameEndsWith(const std::string& name, const std::vector<std::string> extensionList) {
	if(name.substr(0, 2) == "._") return false;

	if(name.size() == 0) return false;

	if(extensionList.size() == 0) return true;

	for(int i = 0; i <(int)extensionList.size(); i++) {
		const std::string ext = extensionList.at(i);
		if(strcasecmp(name.c_str() + name.size() - ext.size(), ext.c_str()) == 0) return true;
	}
	return false;
}

bool dirEntryPredicate(const DirEntry& lhs, const DirEntry& rhs) {
	if(!lhs.isDirectory && rhs.isDirectory) {
		return false;
	}
	if(lhs.isDirectory && !rhs.isDirectory) {
		return true;
	}
	return strcasecmp(lhs.name.c_str(), rhs.name.c_str()) < 0;
}

void getDirectoryContents(std::vector<DirEntry>& dirContents, const std::vector<std::string> extensionList) {
	struct stat st;

	dirContents.clear();

	DIR *pdir = opendir(".");

	if(pdir == NULL) {
		DisplayMsg("Unable to open the directory.");
		for(int i=0;i<120;i++)	gspWaitForVBlank();
	} else {
		while(true) {
			DirEntry dirEntry;

			struct dirent* pent = readdir(pdir);
			if(pent == NULL) break;

			stat(pent->d_name, &st);
			dirEntry.name = pent->d_name;
			dirEntry.isDirectory = (st.st_mode & S_IFDIR) ? true : false;

			if(dirEntry.name.compare(".") != 0 && (dirEntry.isDirectory || nameEndsWith(dirEntry.name, extensionList))) {
				dirContents.push_back(dirEntry);
			}
		}
		closedir(pdir);
	}
	sort(dirContents.begin(), dirContents.end(), dirEntryPredicate);
}

void getDirectoryContents(std::vector<DirEntry>& dirContents) {
	getDirectoryContents(dirContents, {});
}

std::vector<std::string> getContents(const std::string &name, const std::vector<std::string> &extensionList) {
	std::vector<std::string> dirContents;
	DIR* pdir = opendir(name.c_str());
	struct dirent *pent;
	while ((pent = readdir(pdir)) != NULL) {
		if(nameEndsWith(pent->d_name, extensionList))
			dirContents.push_back(pent->d_name);
	}
	closedir(pdir);
	return dirContents;
}

// Directory exist?
bool returnIfExist(const std::string &path, const std::vector<std::string> &extensionList) {
	dirContents.clear();
	chdir(path.c_str());
	std::vector<DirEntry> dirContentsTemp;
	getDirectoryContents(dirContentsTemp, extensionList);
	for(uint i=0;i<dirContentsTemp.size();i++) {
		dirContents.push_back(dirContentsTemp[i]);
	}
	if (dirContents.size() == 0) {
		return false;
	}
	return true;
}

// returns a Path or file to 'std::string'.
// selectText is the Text which is displayed on the bottom bar of the top screen.
// selectionMode is how you select it. 1 -> Path, 2 -> File.
std::string selectFilePath(std::string selectText, const std::vector<std::string> &extensionList, int selectionMode) {
	static uint selectedFile = 0;
	std::string selectedPath = "";
	static int keyRepeatDelay = 4;
	static bool dirChanged = true;
	static bool fastMode = false;
	uint screenPos = 0;
	uint screenPosList = 0;
	std::vector<DirEntry> dirContents;
	std::string dirs;

	while (1) {
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, BLACK);
		C2D_TargetClear(bottom, BLACK);
		Gui::DrawTop();
		char path[PATH_MAX];
		getcwd(path, PATH_MAX);
		if (Config::UseBars == true) {
			Gui::DrawString((400-(Gui::GetStringWidth(0.60f, path)))/2, 2, 0.60f, Config::TxtColor, path, 390);
			Gui::DrawStringCentered(0, 220, 0.60f, Config::TxtColor, selectText, 390);
		} else {
			Gui::DrawString((400-(Gui::GetStringWidth(0.60f, path)))/2, 0, 0.60f, Config::TxtColor, path, 390);
			Gui::DrawStringCentered(0, 218, 0.60f, Config::TxtColor, selectText, 390);
		}
		Gui::DrawBottom();
		if (Config::viewMode == 0) {
			for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)dirContents.size();i++) {
				dirs = dirContents[screenPos + i].name;
				if(screenPos + i == selectedFile) {
					Gui::Draw_Rect(0, 40+(i*57), 320, 45, C2D_Color32(120, 192, 216, 255));
				} else { 
					Gui::Draw_Rect(0, 40+(i*57), 320, 45, C2D_Color32(77, 118, 132, 255));
				}
				Gui::DrawStringCentered(0, 50+(i*57), 0.7f, WHITE, dirs, 320);
			}
		} else if (Config::viewMode == 1) {
			for(int i=0;i<ENTRIES_PER_LIST && i<(int)dirContents.size();i++) {
				dirs = dirContents[screenPosList + i].name;
				if(screenPosList + i == selectedFile) {
					Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::SelectedColor);
				} else {
					Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::UnselectedColor);
				}
				Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, Config::TxtColor, dirs, 320);
			}
		}

		if (Config::UseBars == true) {
			Gui::DrawStringCentered(0, 0, 0.45f, Config::TxtColor, Lang::get("FILEBROWSE_MSG"), 260);
		} else {
			Gui::DrawStringCentered(0, 2, 0.45f, Config::TxtColor, Lang::get("FILEBROWSE_MSG"), 260);
		}
		Gui::DrawArrow(295, -1);
		Gui::DrawArrow(315, 240, 180.0);
		Gui::spriteBlend(sprites_view_idx, buttonPositions[6].x, buttonPositions[6].y);
		
		Gui::Draw_Rect(buttonPositions[2].x, buttonPositions[2].y, buttonPositions[2].w, buttonPositions[2].h, C2D_Color32(0, 0, 0, 190));
		Gui::Draw_Rect(buttonPositions[3].x, buttonPositions[3].y, buttonPositions[3].w, buttonPositions[3].h, C2D_Color32(0, 0, 0, 190));
		Gui::Draw_Rect(buttonPositions[4].x, buttonPositions[4].y, buttonPositions[4].w, buttonPositions[4].h, C2D_Color32(0, 0, 0, 190));
		Gui::Draw_Rect(buttonPositions[5].x, buttonPositions[5].y, buttonPositions[5].w, buttonPositions[5].h, C2D_Color32(0, 0, 0, 190));

		Gui::DrawStringCentered(-120, 222, 0.4, Config::TxtColor, Lang::get("OPEN"), 40);
		Gui::DrawStringCentered(-55, 222, 0.4, Config::TxtColor, Lang::get("SELECT"), 40);
		Gui::DrawStringCentered(10, 222, 0.4, Config::TxtColor, Lang::get("REFRESH"), 40);
		Gui::DrawStringCentered(75, 222, 0.4, Config::TxtColor, Lang::get("BACK"), 40);
		C3D_FrameEnd(0);

		// The input part.
		gspWaitForVBlank();
		hidScanInput();
		hidTouchRead(&touch);

		if (keyRepeatDelay)	keyRepeatDelay--;

		if (dirChanged) {
			dirContents.clear();
			std::vector<DirEntry> dirContentsTemp;
			getDirectoryContents(dirContentsTemp, extensionList);
			for(uint i=0;i<dirContentsTemp.size();i++) {
				dirContents.push_back(dirContentsTemp[i]);
			}
			dirChanged = false;
		}

		if ((hidKeysDown() & KEY_SELECT) || (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[4]))) {
			dirChanged = true;
		}

		if ((hidKeysDown() & KEY_A) || (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[2]))) {
			if (dirContents.size() != 0) {
				if (dirContents[selectedFile].isDirectory) {
					chdir(dirContents[selectedFile].name.c_str());
					selectedFile = 0;
					dirChanged = true;
				}
			}
		}

		if (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[0])) {
			if (selectedFile > 0)	selectedFile--;
		}

		if (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[1])) {
			if (selectedFile < dirContents.size()-1)	selectedFile++;
		}

		if (hidKeysHeld() & KEY_UP) {
			if (selectedFile > 0 && !keyRepeatDelay) {
				selectedFile--;
				if (fastMode == true) {
					keyRepeatDelay = 3;
				} else if (fastMode == false){
					keyRepeatDelay = 6;
				}
			}
		}
		if (hidKeysHeld() & KEY_DOWN && !keyRepeatDelay) {
			if (selectedFile < dirContents.size()-1) {
				selectedFile++;
				if (fastMode == true) {
					keyRepeatDelay = 3;
				} else if (fastMode == false){
					keyRepeatDelay = 6;
				}
			}
		}

		if ((hidKeysDown() & KEY_B) || (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[5]))) {
			char path[PATH_MAX];
			getcwd(path, PATH_MAX);
			if(strcmp(path, "sdmc:/") == 0 || strcmp(path, "/") == 0) {
				return "";
			} else {
				chdir("..");
				selectedFile = 0;
				dirChanged = true;
			}
		}

		if ((hidKeysDown() & KEY_X) || (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[3]))) {
			char path[PATH_MAX];
			getcwd(path, PATH_MAX);
			selectedPath = path;
			if (selectionMode == 2) {
				selectedPath += dirContents[selectedFile].name;
			}
			return selectedPath;
		}

		if (hidKeysDown() & KEY_R) {
			fastMode = true;
		}

		if (hidKeysDown() & KEY_L) {
			fastMode = false;
		}
		// Switch ViewMode.
		if ((hidKeysDown() & KEY_Y) || (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[6]))) {
			if (Config::viewMode == 0) {
				Config::viewMode = 1;
			} else {
				Config::viewMode = 0;
			}
		}

		if (Config::viewMode == 0) {
			if(selectedFile < screenPos) {
				screenPos = selectedFile;
			} else if (selectedFile > screenPos + ENTRIES_PER_SCREEN - 1) {
				screenPos = selectedFile - ENTRIES_PER_SCREEN + 1;
			}
		} else if (Config::viewMode == 1) {
			if(selectedFile < screenPosList) {
				screenPosList = selectedFile;
			} else if (selectedFile > screenPosList + ENTRIES_PER_LIST - 1) {
				screenPosList = selectedFile - ENTRIES_PER_LIST + 1;
			}
		}
	}
}