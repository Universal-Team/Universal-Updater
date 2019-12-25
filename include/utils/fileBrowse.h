#ifndef FILE_BROWSE_H
#define FILE_BROWSE_H

#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <vector>

using namespace std;

struct DirEntry {
	std::string name;
	std::string path;
	bool isDirectory;
	char tid[5];
	off_t size;
};

typedef struct {
	char gameTitle[12];			//!< 12 characters for the game title.
	char gameCode[4];			//!< 4 characters for the game code.
} sNDSHeadertitlecodeonly;

void findNdsFiles(vector<DirEntry>& dirContents);

bool nameEndsWith(const std::string& name, const std::vector<std::string> extensionList);
void getDirectoryContents(std::vector<DirEntry>& dirContents, const std::vector<std::string> extensionList);
void getDirectoryContents(std::vector<DirEntry>& dirContents);
std::vector<std::string> getContents(const std::string &name, const std::vector<std::string> &extensionList);

std::string selectFilePath(std::string selectText, int selectionMode = 1);

#endif //FILE_BROWSE_H