/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "config.hpp"
#include "download.hpp"
#include "formatting.hpp"
#include "gui.hpp"
#include "keyboard.hpp"
#include "lang.hpp"
#include "screenCommon.hpp"
#include "thread.hpp"

#include <string>
#include <vector>

#define  USER_AGENT   APP_TITLE "-" VERSION_STRING

static char* result_buf = NULL;
static size_t result_sz = 0;
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
extern u32 TextColor;

curl_off_t downloadTotal = 1; //Dont initialize with 0 to avoid division by zero later
curl_off_t downloadNow = 0;

static FILE *downfile = NULL;
static size_t file_buffer_pos = 0;
static size_t file_toCommit_size = 0;
static char* g_buffers[2] = { NULL };
static u8 g_index = 0;
static Thread fsCommitThread;
static LightEvent readyToCommit;
static LightEvent waitCommit;
static bool killThread = false;
static bool writeError = false;
#define FILE_ALLOC_SIZE 0x60000

static int curlProgress(CURL *hnd,
					curl_off_t dltotal, curl_off_t dlnow,
					curl_off_t ultotal, curl_off_t ulnow)
{
	downloadTotal = dltotal;
	downloadNow = dlnow;

	return 0;
}

bool filecommit() {
	if (!downfile) return false;
	fseek(downfile, 0, SEEK_END);
	u32 byteswritten = fwrite(g_buffers[!g_index], 1, file_toCommit_size, downfile);
	if (byteswritten != file_toCommit_size) return false;
	file_toCommit_size = 0;
	return true;
}

static void commitToFileThreadFunc(void* args) {
	LightEvent_Signal(&waitCommit);
	while (true) {
		LightEvent_Wait(&readyToCommit);
		LightEvent_Clear(&readyToCommit);
		if (killThread) threadExit(0);
		writeError = !filecommit();
		LightEvent_Signal(&waitCommit);
	}
}

static size_t file_handle_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
	(void)userdata;
	const size_t bsz = size * nmemb;
	size_t tofill = 0;
	if (writeError) return 0;
	if (!g_buffers[g_index]) {

		LightEvent_Init(&waitCommit, RESET_STICKY);
		LightEvent_Init(&readyToCommit, RESET_STICKY);

		s32 prio = 0;
		svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
		fsCommitThread = threadCreate(commitToFileThreadFunc, NULL, 0x1000, prio - 1, -2, true);

		g_buffers[0] = (char*)memalign(0x1000, FILE_ALLOC_SIZE);
		g_buffers[1] = (char*)memalign(0x1000, FILE_ALLOC_SIZE);

		if (!fsCommitThread || !g_buffers[0] || !g_buffers[1]) return 0;
	}
	if (file_buffer_pos + bsz >= FILE_ALLOC_SIZE) {
		tofill = FILE_ALLOC_SIZE - file_buffer_pos;
		memcpy(g_buffers[g_index] + file_buffer_pos, ptr, tofill);
		
		LightEvent_Wait(&waitCommit);
		LightEvent_Clear(&waitCommit);
		file_toCommit_size = file_buffer_pos + tofill;
		file_buffer_pos = 0;
		svcFlushProcessDataCache(CUR_PROCESS_HANDLE, g_buffers[g_index], file_toCommit_size);
		g_index = !g_index;
		LightEvent_Signal(&readyToCommit);
	}
	memcpy(g_buffers[g_index] + file_buffer_pos, ptr + tofill, bsz - tofill);
	file_buffer_pos += bsz - tofill;
	return bsz;
}

Result downloadToFile(std::string url, std::string path) {

	Result retcode = 0;
	downloadTotal = 1;
	downloadNow = 0;
	int res;
	CURL *hnd;
	CURLcode cres;

	printf("Downloading from:\n%s\nto:\n%s\n", url.c_str(), path.c_str());
	const char* filepath = path.c_str();

	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) {
		retcode = -1;
		goto exit;
	}
	
	res = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(res)) {
		retcode = res;
		goto exit;
	}

	makeDirs(strdup(filepath));
	downfile = fopen(filepath, "wb");
	if (!downfile) {
		retcode = -2;
		goto exit;
	}

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, FILE_ALLOC_SIZE);
	curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 0L); 
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(hnd, CURLOPT_FAILONERROR, 1L);
	curl_easy_setopt(hnd, CURLOPT_ACCEPT_ENCODING, "gzip");
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, curlProgress);
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, file_handle_data);
	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(hnd, CURLOPT_STDERR, stdout);

	cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	
	if (cres != CURLE_OK) {
		retcode = -cres;
		goto exit;
	}

	LightEvent_Wait(&waitCommit);
	LightEvent_Clear(&waitCommit);

	file_toCommit_size = file_buffer_pos;
	svcFlushProcessDataCache(CUR_PROCESS_HANDLE, g_buffers[g_index], file_toCommit_size);
	g_index = !g_index;
	if (!filecommit()) {
		retcode = -3;
		goto exit;
	}
	fflush(downfile);
	
exit:
	if (fsCommitThread) {
		killThread = true;
		LightEvent_Signal(&readyToCommit);
		threadJoin(fsCommitThread, U64_MAX);
		killThread = false;
		fsCommitThread = NULL;
	}

	socExit();
	
	if (socubuf) {
		free(socubuf);
	}
	if (downfile) {
		fclose(downfile);
		downfile = NULL;
	}
	if (g_buffers[0]) {
		free(g_buffers[0]);
		g_buffers[0] = NULL;
	}
	if (g_buffers[1]) {
		free(g_buffers[1]);
		g_buffers[1] = NULL;
	}
	g_index = 0;
	file_buffer_pos = 0;
	file_toCommit_size = 0;
	writeError = false;
	
	return retcode;
}

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

// Fetch GitHub Releases.
std::vector<ReleaseFetch> fetchReleases(nlohmann::json API) {
	ReleaseFetch fetch[API.size()];
	std::vector<ReleaseFetch> fetchVector;

	for (int i = 0; i < (int)API.size(); i++) {
		// Get Stuff.
		fetch[i].Target = (std::string)API[i]["target_commitish"];
		fetch[i].TagName = (std::string)API[i]["tag_name"];
		fetch[i].ReleaseName = (std::string)API[i]["name"];
		fetch[i].Created = (std::string)API[i]["created_at"];
		fetch[i].Published = (std::string)API[i]["published_at"];
		fetch[i].PreRelease = API[i]["prerelease"];
		// Push to the Vector.
		fetchVector.push_back(fetch[i]);
	}
	return fetchVector;
}

extern touchPosition touch;
extern bool touching(touchPosition touch, Structs::ButtonPos button);
std::vector<Structs::ButtonPos> arrowPos = {
	{295, 0, 25, 25}, // Arrow Up.
	{295, 215, 25, 25} // Arrow Down.
};

int SelectRelease(std::vector<ReleaseFetch> bruh) {
	std::string line1;
	std::string line2;
	int selectedRelease = 0;
	int keyRepeatDelay = 4;
	bool fastMode = false;
	int screenPos = 0;
	int screenPosList = 0;

	while (1) {
		std::string releaseAmount = std::to_string(selectedRelease+1) + " | " + std::to_string(bruh.size());
		// Draw Part.
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, BLACK);
		C2D_TargetClear(Bottom, BLACK);
		GFX::DrawTop();
		if (Config::UseBars == true) {
			Gui::DrawStringCentered(0, 0, 0.7f, TextColor, Lang::get("VERSION_SELECT"), 400);
			Gui::DrawString(397-Gui::GetStringWidth(0.6f, releaseAmount), 239-Gui::GetStringHeight(0.6f, releaseAmount), 0.6f, TextColor, releaseAmount);
		} else {
			Gui::DrawStringCentered(0, 2, 0.7f, TextColor, Lang::get("VERSION_SELECT"), 400);
			Gui::DrawString(397-Gui::GetStringWidth(0.6f, releaseAmount), 237-Gui::GetStringHeight(0.6f, releaseAmount), 0.6f, TextColor, releaseAmount);
		}
		// Display Informations.
		Gui::DrawStringCentered(0, 35, 0.7f, TextColor, Lang::get("TAG_NAME") + std::string(bruh[selectedRelease].TagName), 400);
		Gui::DrawStringCentered(0, 65, 0.7f, TextColor, Lang::get("TARGET") + std::string(bruh[selectedRelease].Target), 400);
		Gui::DrawStringCentered(0, 95, 0.7f, TextColor, Lang::get("RELEASE_NAME") + std::string(bruh[selectedRelease].ReleaseName), 400);
		Gui::DrawStringCentered(0, 125, 0.7f, TextColor, Lang::get("CREATED_AT") + std::string(bruh[selectedRelease].Created), 400);
		Gui::DrawStringCentered(0, 155, 0.7f, TextColor, Lang::get("PUBLISHED_AT") + std::string(bruh[selectedRelease].Published), 400);
		if (bruh[selectedRelease].PreRelease)	Gui::DrawStringCentered(0, 185, 0.7f, TextColor, Lang::get("IS_PRERELEASE") + Lang::get("YES"), 400);
		else	Gui::DrawStringCentered(0, 185, 0.7f, TextColor, Lang::get("IS_PRERELEASE") + Lang::get("NO"), 400);

		GFX::DrawBottom();
		GFX::DrawArrow(295, -1);
		GFX::DrawArrow(315, 240, 180.0);

		if (Config::viewMode == 0) {
			for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)bruh.size();i++) {
				Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::UnselectedColor);
				line1 = bruh[screenPos + i].TagName;
				line2 = bruh[screenPos + i].Published.substr(0, 10);
				if(screenPos + i == selectedRelease) {
					Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, Config::SelectedColor);
				}
				Gui::DrawStringCentered(0, 38+(i*57), 0.7f, Config::TxtColor, line1, 320);
				Gui::DrawStringCentered(0, 62+(i*57), 0.7f, Config::TxtColor, line2, 320);
			}
		} else if (Config::viewMode == 1) {
			for(int i=0;i<ENTRIES_PER_LIST && i<(int)bruh.size();i++) {
				Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::UnselectedColor);
				line1 = bruh[screenPosList + i].TagName;
				if(screenPosList + i == selectedRelease) {
					Gui::drawAnimatedSelector(0, (i+1)*27, 320, 25, .060, TRANSPARENT, Config::SelectedColor);
				}
				Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, Config::TxtColor, line1, 320);
			}
		}
		C3D_FrameEnd(0);

		// The input part.
		hidScanInput();
		u32 hDown = hidKeysDown();
		u32 hHeld = hidKeysHeld();
		hidTouchRead(&touch);
		if (keyRepeatDelay)	keyRepeatDelay--;

		if (hidKeysDown() & KEY_Y) {
			if (Config::viewMode == 0) {
				Config::viewMode = 1;
			} else {
				Config::viewMode = 0;
			}
		}

		if (hDown & KEY_A) {
			return (int)selectedRelease;
		}

		if (hDown & KEY_B) {
			return 0;
		}

		if (hidKeysDown() & KEY_TOUCH && touching(touch, arrowPos[0])) {
			if (selectedRelease > 0)	selectedRelease--;
		}

		if (hidKeysDown() & KEY_TOUCH && touching(touch, arrowPos[1])) {
			if ((uint)selectedRelease < bruh.size()-1)	selectedRelease++;
		}

		if (hHeld & KEY_UP) {
			if (selectedRelease > 0 && !keyRepeatDelay) {
				selectedRelease--;
				if (fastMode == true) {
					keyRepeatDelay = 3;
				} else if (fastMode == false){
					keyRepeatDelay = 6;
				}
			}
		} else if (hHeld & KEY_DOWN && !keyRepeatDelay) {
			if ((uint)selectedRelease < bruh.size()-1) {
				selectedRelease++;
				if (fastMode == true) {
					keyRepeatDelay = 3;
				} else if (fastMode == false){
					keyRepeatDelay = 6;
				}
			}
		}

		if (hidKeysDown() & KEY_R) {
			fastMode = true;
		}

		if (hidKeysDown() & KEY_L) {
			fastMode = false;
		}

		if (hDown & KEY_START) {
			return -2; // Cancel.
		}

		if (hDown & KEY_TOUCH) {
			if (Config::viewMode == 0) {
				for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)bruh.size(); i++) {
					if(touch.py > 40+(i*57) && touch.py < 40+(i*57)+45) {
						if (bruh.size() != 0) {
							return screenPos + i;
						}
					}
				}
			} else if (Config::viewMode == 1) {
				for(int i=0;i<ENTRIES_PER_LIST && i<(int)bruh.size(); i++) {
					if(touch.py > (i+1)*27 && touch.py < (i+2)*27) {
						if (bruh.size() != 0) {
							return screenPosList + i;
						}
					}
				}
			}
		}

		if (Config::viewMode == 0) {
			if(selectedRelease < screenPos) {
				screenPos = selectedRelease;
			} else if (selectedRelease > screenPos + ENTRIES_PER_SCREEN - 1) {
				screenPos = selectedRelease - ENTRIES_PER_SCREEN + 1;
			}
		} else if (Config::viewMode == 1) {
			if(selectedRelease < screenPosList) {
				screenPosList = selectedRelease;
			} else if (selectedRelease > screenPosList + ENTRIES_PER_LIST - 1) {
				screenPosList = selectedRelease - ENTRIES_PER_LIST + 1;
			}
		}
	}
}

Result downloadFromRelease(std::string url, std::string asset, std::string path, std::string Message, bool includePrereleases, bool showVersions)
{
	Result ret = 0;
	// Do not display progressbar.
	if (showVersions) {
		showProgressBar = false;
		Msg::DisplayMsg(Lang::get("FETCHING_RELEASES"));
	}

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
	apiurlStream << "https://api.github.com/repos/" << repoOwner << "/" << repoName << (includePrereleases || showVersions ? "/releases" : "/releases/latest");
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
	if(showVersions) {
		if(!includePrereleases) {
			for(auto it = parsedAPI.begin(); it != parsedAPI.end();) {
				if((*it)["prerelease"]) {
					parsedAPI.erase(it);
				} else {
					it++;
				}
			}
		}
		if(parsedAPI.size() == 0) {
			// All were prereleases and those are being ignored
			return -2; // TODO: Maybe change this? I'm note sure what good return values are -Pk11
		}
		std::vector<ReleaseFetch> fetchResult = fetchReleases(parsedAPI);
		int release = SelectRelease(fetchResult);

		if (release == -2) {
			socExit();
			free(result_buf);
			free(socubuf);
			result_buf = NULL;
			result_sz = 0;
			result_written = 0;
			return -1;
		}
		
		parsedAPI = parsedAPI[release];
	} else if(includePrereleases) {
		parsedAPI = parsedAPI[0];
	}
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
	if (assetUrl.empty()) {
		ret = DL_ERROR_GIT;
	} else {
		snprintf(progressBarMsg, sizeof(progressBarMsg), Message.c_str());
		showProgressBar = true;
		progressBarType = 0;
		Threads::create((ThreadFunc)displayProgressBar);
		ret = downloadToFile(assetUrl, path);
	}
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
	Msg::DisplayMsg(Lang::get("DOWNLOAD_FAILED"));
	for (int i = 0; i < 60*2; i++) {
		gspWaitForVBlank();
	}
}

void notImplemented(void) {
	Msg::DisplayMsg(Lang::get("NOT_IMPLEMENTED"));
	for (int i = 0; i < 60*2; i++) {
		gspWaitForVBlank();
	}
}

void doneMsg(void) {
	Msg::DisplayMsg(Lang::get("DONE"));
	for (int i = 0; i < 60*2; i++) {
		gspWaitForVBlank();
	}
}

void notConnectedMsg(void) {
	Msg::DisplayMsg(Lang::get("CONNECT_WIFI"));
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
		C2D_TargetClear(Top, BLACK);
		C2D_TargetClear(Bottom, BLACK);
		GFX::DrawTop();
		Gui::DrawStringCentered(0, 1, 0.7f, TextColor, progressBarMsg, 400);

		// Display 'Currently Extracting: <Filename>'.
		if (progressBarType == 1) {
			Gui::DrawStringCentered(0, 140, 0.6f, TextColor, str, 400);
			Gui::DrawStringCentered(0, 60, 0.6f, TextColor, Lang::get("CURRENTLY_EXTRACTING") + extractingFile, 400);
		}

		// Only display this by downloading.
		if (progressBarType == 0) {
			Gui::DrawStringCentered(0, 80, 0.6f, TextColor, str, 400);
			Gui::Draw_Rect(30, 120, 340, 30, BLACK);
			if (isScriptSelected == true) {
				Animation::DrawProgressBar(downloadNow, downloadTotal, 1);
			} else {
				Animation::DrawProgressBar(downloadNow, downloadTotal, 2);
			}
		}
		GFX::DrawBottom();
		C3D_FrameEnd(0);
		gspWaitForVBlank();
	}
}