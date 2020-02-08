#ifndef FILE_BROWSE_HPP
#define FILE_BROWSE_HPP

#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <vector>

using namespace std;

struct DirEntry {
	std::string name;
	std::string path;
	bool isDirectory;
	off_t size;
};

bool nameEndsWith(const std::string& name, const std::vector<std::string> extensionList);
void getDirectoryContents(std::vector<DirEntry>& dirContents, const std::vector<std::string> extensionList);
void getDirectoryContents(std::vector<DirEntry>& dirContents);
std::vector<std::string> getContents(const std::string &name, const std::vector<std::string> &extensionList);

bool returnIfExist(const std::string &path, const std::vector<std::string> &extensionList);

std::string selectFilePath(std::string selectText, const std::vector<std::string> &extensionList, int selectionMode = 1);

#endif //FILE_BROWSE_HPP