/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "gui.hpp"

#include "download/download.hpp"

#include "lang/lang.hpp"

#include "screens/screenCommon.hpp"

#include "utils/config.hpp"
#include "utils/formatting.hpp"

#include <vector>
#include <string>

#define  USER_AGENT   APP_TITLE "-" VERSION_STRING

static char* result_buf = NULL;
static size_t result_sz = 0;
static Handle* result_fileHandle = nullptr;
static size_t result_written = 0;
std::vector<std::string> _topText;
std::string jsonName;

extern bool downloadNightlies;
extern int filesExtracted;
extern std::string extractingFile;

char progressBarMsg[128] = "";
bool showProgressBar = false;
bool progressBarType = 0; // 0 = Download | 1 = Extract

#define TIME_IN_US 1  
#define TIMETYPE curl_off_t
#define TIMEOPT CURLINFO_TOTAL_TIME_T
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3000000

extern u32 progressBar;
extern bool isScriptSelected;

// following function is from
// https://github.com/angelsl/libctrfgh/blob/master/curl_test/src/main.c
static size_t handle_data(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	(void) userdata;
	const size_t bsz = size*nmemb;

	if (result_sz == 0 || !result_buf)
	{
		result_sz = 0x1000;
		result_buf = (char*)malloc(result_sz);
	}

	bool need_realloc = false;
	while (result_written + bsz > result_sz)
	{
		result_sz <<= 1;
		need_realloc = true;
	}

	if (need_realloc)
	{
		char *new_buf = (char*)realloc(result_buf, result_sz);
		if (!new_buf)
		{
			return 0;
		}
		result_buf = new_buf;
	}

	if (!result_buf)
	{
		return 0;
	}

	memcpy(result_buf + result_written, ptr, bsz);
	result_written += bsz;
	return bsz;
}

static size_t handle_data_to_file(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	(void) userdata;
	const size_t bsz = size*nmemb;

	u32 bytesWritten = 0;
	FSFILE_Write(*result_fileHandle, &bytesWritten, result_written, ptr, bsz, 0);

	result_written += bsz;
	return bsz;
}

curl_off_t downloadTotal = 1; //Dont initialize with 0 to avoid division by zero later
curl_off_t downloadNow = 0;

static int curlProgress(CURL *hnd,
					curl_off_t dltotal, curl_off_t dlnow,
					curl_off_t ultotal, curl_off_t ulnow)
{
	downloadTotal = dltotal;
	downloadNow = dlnow;

	return 0;
}

static Result setupContext(CURL *hnd, const char * url)
{
	downloadTotal = 1;
	downloadNow = 0;
	curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, curlProgress);

	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, handle_data);
	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(hnd, CURLOPT_STDERR, stdout);

	return 0;
}

static Result setupContextForDirectToFileDownload(CURL *hnd, const char * url)
{
	Result ret = setupContext(hnd, url);
	if (ret != 0) {
		return ret;
	}

	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, handle_data_to_file);

	return 0;
}

Result downloadToFile(std::string url, std::string path)
{
	Result ret = 0;
	printf("Downloading from:\n%s\nto:\n%s\n", url.c_str(), path.c_str());

	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf)
	{
		return -1;
	}

	ret = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(ret))
	{
		free(socubuf);
		return ret;
	}

	Handle fileHandle;

	ret = openFile(&fileHandle, path.c_str(), true);
	if (R_FAILED(ret)) {
		printf("Error: couldn't open file to write.\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return DL_ERROR_WRITEFILE;
	}

	result_fileHandle = &fileHandle;

	CURL *hnd = curl_easy_init();
	ret = setupContextForDirectToFileDownload(hnd, url.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_fileHandle = nullptr;
		result_sz = 0;
		result_written = 0;
		FSFILE_Close(fileHandle);
		return ret;
	}

	u64 startTime = osGetTime();

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_fileHandle = nullptr;
		result_sz = 0;
		result_written = 0;
		FSFILE_Close(fileHandle);
		return -1;
	}

	u64 endTime = osGetTime();
	u64 totalTime = endTime - startTime;
	printf("Download took %llu milliseconds.\n", totalTime);

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = NULL;
	result_fileHandle = nullptr;
	result_sz = 0;
	result_written = 0;
	FSFILE_Close(fileHandle);
	return 0;
}

Result downloadFromRelease(std::string url, std::string asset, std::string path, bool includePrereleases)
{
	Result ret = 0;
	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf)
	{
		return -1;
	}

	ret = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(ret))
	{
		free(socubuf);
		return ret;
	}

	std::regex parseUrl("github\\.com\\/(.+)\\/(.+)");
	std::smatch result;
	regex_search(url, result, parseUrl);

	std::string repoOwner = result[1].str(), repoName = result[2].str();

	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repoOwner << "/" << repoName << (includePrereleases ? "/releases" : "/releases/latest");
	std::string apiurl = apiurlStream.str();

	printf("Downloading latest release from repo:\n%s\nby:\n%s\n", repoName.c_str(), repoOwner.c_str());
	printf("Crafted API url:\n%s\n", apiurl.c_str());

	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return ret;
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return -1;
	}

	printf("Looking for asset with matching name:\n%s\n", asset.c_str());
	std::string assetUrl;
	json parsedAPI = json::parse(result_buf);
	if(includePrereleases)	parsedAPI = parsedAPI[0];
	if (parsedAPI["assets"].is_array()) {
		for (auto jsonAsset : parsedAPI["assets"]) {
			if (jsonAsset.is_object() && jsonAsset["name"].is_string() && jsonAsset["browser_download_url"].is_string()) {
				std::string assetName = jsonAsset["name"];
				if (matchPattern(asset, assetName)) {
					assetUrl = jsonAsset["browser_download_url"];
					break;
				}
			}
		}
	}
	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;

	if (assetUrl.empty())
		ret = DL_ERROR_GIT;
	else
		ret = downloadToFile(assetUrl, path);

	return ret;
}

/**
 * Check Wi-Fi status.
 * @return True if Wi-Fi is connected; false if not.
 */
bool checkWifiStatus(void) {
	u32 wifiStatus;
	bool res = false;

	if (R_SUCCEEDED(ACU_GetWifiStatus(&wifiStatus)) && wifiStatus) {
		res = true;
	}

	return res;
}

void downloadFailed(void) {
	DisplayMsg(Lang::get("DOWNLOAD_FAILED"));
	for (int i = 0; i < 60*2; i++) {
		gspWaitForVBlank();
	}
}

void notImplemented(void) {
	DisplayMsg(Lang::get("NOT_IMPLEMENTED"));
	for (int i = 0; i < 60*2; i++) {
		gspWaitForVBlank();
	}
}

void doneMsg(void) {
	DisplayMsg(Lang::get("DONE"));
	for (int i = 0; i < 60*2; i++) {
		gspWaitForVBlank();
	}
}

void notConnectedMsg(void) {
	DisplayMsg(Lang::get("CONNECT_WIFI"));
	for (int i = 0; i < 60*2; i++) {
		gspWaitForVBlank();
	}
}

std::string getLatestRelease(std::string repo, std::string item)
{
	Result ret = 0;
	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf)
	{
		return "";
	}

	ret = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(ret))
	{
		free(socubuf);
		return "";
	}

	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repo << "/releases/latest";
	std::string apiurl = apiurlStream.str();

	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}

	std::string jsonItem;
	json parsedAPI = json::parse(result_buf);
	if (parsedAPI[item].is_string()) {
		jsonItem = parsedAPI[item];
	}
	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;

	return jsonItem;
}

std::string getLatestCommit(std::string repo, std::string item)
{
	Result ret = 0;
	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf)
	{
		return "";
	}

	ret = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(ret))
	{
		free(socubuf);
		return "";
	}

	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repo << "/commits/master";
	std::string apiurl = apiurlStream.str();

	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}

	std::string jsonItem;
	json parsedAPI = json::parse(result_buf);
	if (parsedAPI[item].is_string()) {
		jsonItem = parsedAPI[item];
	}
	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;

	return jsonItem;
}

std::string getLatestCommit(std::string repo, std::string array, std::string item)
{
	Result ret = 0;
	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf)
	{
		return "";
	}

	ret = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(ret))
	{
		free(socubuf);
		return "";
	}

	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repo << "/commits/master";
	std::string apiurl = apiurlStream.str();

	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}

	std::string jsonItem;
	json parsedAPI = json::parse(result_buf);
	if (parsedAPI[array][item].is_string()) {
		jsonItem = parsedAPI[array][item];
	}
	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;

	return jsonItem;
}

std::vector<ListEntry> getList(std::string repo, std::string path)
{
	Result ret = 0;
	void *socubuf = memalign(0x1000, 0x100000);
	std::vector<ListEntry> emptyVector;
	if (!socubuf)
	{
		return emptyVector;
	}

	ret = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(ret))
	{
		free(socubuf);
		return emptyVector;
	}

	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repo << "/contents/" << path;
	std::string apiurl = apiurlStream.str();

	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return emptyVector;
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;

		return emptyVector;
	}

	std::vector<ListEntry> jsonItems;
	json parsedAPI = json::parse(result_buf);
	for(uint i=0;i<parsedAPI.size();i++) {
		ListEntry listEntry;
		if (parsedAPI[i]["name"].is_string()) {
			listEntry.name = parsedAPI[i]["name"];
		}
		if (parsedAPI[i]["download_url"].is_string()) {
			listEntry.downloadUrl = parsedAPI[i]["download_url"];
		}
		if (parsedAPI[i]["path"].is_string()) {
			listEntry.sdPath = "sdmc:/";
			listEntry.sdPath += parsedAPI[i]["path"];
			listEntry.path = parsedAPI[i]["path"];

			size_t pos;
			while ((pos = listEntry.path.find(" ")) != std::string::npos) {
				listEntry.path.replace(pos, 1, "%20");
			}
		}
		jsonItems.push_back(listEntry);
	}

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;

	return jsonItems;
}

void displayProgressBar() {
	char str[256];
	while(showProgressBar) {
		if (downloadTotal < 1.0f) {
			downloadTotal = 1.0f;
		}
		if (downloadTotal < downloadNow) {
			downloadTotal = downloadNow;
		}

		if (progressBarType) {
			snprintf(str, sizeof(str), "%i %s",  
					filesExtracted, 
					(filesExtracted == 1 ? (Lang::get("FILE_EXTRACTED")).c_str() :(Lang::get("FILES_EXTRACTED")).c_str())
					);
		} else {
			snprintf(str, sizeof(str), "%s / %s (%.2f%%)",  
					formatBytes(downloadNow).c_str(),
					formatBytes(downloadTotal).c_str(),
					((float)downloadNow/(float)downloadTotal) * 100.0f
					);
		}

		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, BLACK);
		C2D_TargetClear(bottom, BLACK);
		Gui::DrawTop();
		Gui::DrawStringCentered(0, 1, 0.7f, Config::TxtColor, progressBarMsg, 400);

		Gui::DrawStringCentered(0, 80, 0.6f, Config::TxtColor, str, 400);
		Gui::Draw_Rect(30, 120, 340, 30, BLACK);
		if (isScriptSelected == true) {
			Gui::Draw_Rect(31, 121, (int)(((float)downloadNow/(float)downloadTotal) * 338.0f), 28, progressBar);
		} else {
			Gui::Draw_Rect(31, 121, (int)(((float)downloadNow/(float)downloadTotal) * 338.0f), 28, Config::progressbarColor);
		}
		Gui::DrawBottom();
		C3D_FrameEnd(0);
		gspWaitForVBlank();
	}
}