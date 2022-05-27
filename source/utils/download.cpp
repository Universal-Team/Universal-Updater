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

#include "animation.hpp"
#include "download.hpp"
#include "files.hpp"
#include "json.hpp"
#include "lang.hpp"
#include "queueSystem.hpp"
#include "screenshot.hpp"
#include "scriptUtils.hpp"
#include "stringutils.hpp"
#include "version.hpp"

#include <3ds.h>
#include <curl/curl.h>
#include <dirent.h>
#include <malloc.h>
#include <regex>
#include <string>
#include <unistd.h>
#include <vector>

#define USER_AGENT APP_TITLE "-" VER_NUMBER

static char *result_buf = nullptr;
static size_t result_sz = 0;
static size_t result_written = 0;

#define TIME_IN_US 1
#define TIMETYPE curl_off_t
#define TIMEOPT CURLINFO_TOTAL_TIME_T
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL	3000000

curl_off_t downloadTotal = 1; // Dont initialize with 0 to avoid division by zero later.
curl_off_t downloadNow = 0;
curl_off_t downloadSpeed = 0;

static FILE *downfile = nullptr;
static size_t file_buffer_pos = 0;
static size_t file_toCommit_size = 0;
static char *g_buffers[2] = { nullptr };
static u8 g_index = 0;
static Thread fsCommitThread;
static LightEvent readyToCommit;
static LightEvent waitCommit;
static bool killThread = false;
static bool writeError = false;
#define FILE_ALLOC_SIZE 0x60000
CURL *CurlHandle = nullptr;

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

static void commitToFileThreadFunc(void *args) {
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
	if (getAvailableSpace() < (u64)downloadTotal) return 0; // Out of space.
	if (writeError) return 0;
	if (QueueSystem::CancelCallback) return 0;

	(void)userdata;
	const size_t bsz = size * nmemb;
	size_t tofill = 0;


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
		svcFlushProcessDataCache(CUR_PROCESS_HANDLE, (u32)g_buffers[g_index], file_toCommit_size);
		g_index = !g_index;
		LightEvent_Signal(&readyToCommit);
	}

	memcpy(g_buffers[g_index] + file_buffer_pos, ptr + tofill, bsz - tofill);
	file_buffer_pos += bsz - tofill;
	return bsz;
}

/*
	Download a file.

	const std::string &url: The download URL.
	const std::string &path: Where to place the file.
*/
Result downloadToFile(const std::string &url, const std::string &path) {
	if (!checkWifiStatus()) return -1; // NO WIFI.

	bool needToDelete = false;
	downloadTotal = 1;
	downloadNow = 0;
	downloadSpeed = 0;

	CURLcode curlResult;
	Result retcode = 0;
	int res;

	printf("Downloading from:\n%s\nto:\n%s\n", url.c_str(), path.c_str());

	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) {
		retcode = -1;
		goto exit;
	}

	res = socInit((u32 *)socubuf, 0x100000);
	if (R_FAILED(res)) {
		retcode = res;
		goto exit;
	}

	/* make directories. */
	for (char *slashpos = strchr(path.c_str() + 1, '/'); slashpos != NULL; slashpos = strchr(slashpos + 1, '/')) {
		char bak = *(slashpos);
		*(slashpos) = '\0';

		mkdir(path.c_str(), 0777);

		*(slashpos) = bak;
	}

	downfile = fopen(path.c_str(), "wb");
	if (!downfile) {
		retcode = -2;
		goto exit;
	}

	CurlHandle = curl_easy_init();
	curl_easy_setopt(CurlHandle, CURLOPT_BUFFERSIZE, FILE_ALLOC_SIZE);
	curl_easy_setopt(CurlHandle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(CurlHandle, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(CurlHandle, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(CurlHandle, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(CurlHandle, CURLOPT_FAILONERROR, 1L);
	curl_easy_setopt(CurlHandle, CURLOPT_ACCEPT_ENCODING, "gzip");
	curl_easy_setopt(CurlHandle, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(CurlHandle, CURLOPT_XFERINFOFUNCTION, curlProgress);
	curl_easy_setopt(CurlHandle, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(CurlHandle, CURLOPT_WRITEFUNCTION, file_handle_data);
	curl_easy_setopt(CurlHandle, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(CurlHandle, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(CurlHandle, CURLOPT_STDERR, stdout);

	curlResult = curl_easy_perform(CurlHandle);
	curl_easy_cleanup(CurlHandle);
	CurlHandle = nullptr;

	if (curlResult != CURLE_OK) {
		retcode = -curlResult;
		needToDelete = true;
		goto exit;
	}

	LightEvent_Wait(&waitCommit);
	LightEvent_Clear(&waitCommit);

	file_toCommit_size = file_buffer_pos;
	svcFlushProcessDataCache(CUR_PROCESS_HANDLE, (u32)g_buffers[g_index], file_toCommit_size);
	g_index = !g_index;

	if (!filecommit()) {
		retcode = -3;
		needToDelete = true;
		goto exit;
	}

	fflush(downfile);

exit:
	if (fsCommitThread) {
		killThread = true;
		LightEvent_Signal(&readyToCommit);
		threadJoin(fsCommitThread, U64_MAX);
		killThread = false;
		fsCommitThread = nullptr;
	}

	socExit();

	if (socubuf) free(socubuf);

	if (downfile) {
		fclose(downfile);
		downfile = nullptr;
	}

	if (g_buffers[0]) {
		free(g_buffers[0]);
		g_buffers[0] = nullptr;
	}

	if (g_buffers[1]) {
		free(g_buffers[1]);
		g_buffers[1] = nullptr;
	}

	g_index = 0;
	file_buffer_pos = 0;
	file_toCommit_size = 0;
	writeError = false;

	if (needToDelete) {
		if (access(path.c_str(), F_OK) == 0) deleteFile(path.c_str()); // Delete file, cause not fully downloaded.
	}

	if (QueueSystem::CancelCallback) return 0;
	return retcode;
}

/*
	following function is from
	https://github.com/angelsl/libctrfgh/blob/master/curl_test/src/main.c
*/
static size_t handle_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
	(void)userdata;
	const size_t bsz = size*nmemb;

	if (result_sz == 0 || !result_buf) {
		result_sz = 0x1000;
		result_buf = (char *)malloc(result_sz);
	}

	bool need_realloc = false;
	while (result_written + bsz > result_sz) {
		result_sz <<= 1;
		need_realloc = true;
	}

	if (need_realloc) {
		char *new_buf = (char *)realloc(result_buf, result_sz);
		if (!new_buf) return 0;

		result_buf = new_buf;
	}

	if (!result_buf) return 0;

	memcpy(result_buf + result_written, ptr, bsz);
	result_written += bsz;
	return bsz;
}

/*
	This + Above is Used for No File Write and instead into RAM.
*/
static Result setupContext(CURL *hnd, const char *url) {
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

/*
	Download a file of a GitHub Release.

	const std::string &url: Const Reference to the URL. (https://github.com/Owner/Repo)
	const std::string &asset: Const Reference to the Asset. (File.filetype)
	const std::string &path: Const Reference, where to store. (sdmc:/File.filetype)
	bool includePrereleases: If including Pre-Releases.
*/
Result downloadFromRelease(const std::string &url, const std::string &asset, const std::string &path, bool includePrereleases) {
	Result ret = 0;
	CURL *hnd;

	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) {
		return -1;
	}

	ret = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(ret)) {
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

	hnd = curl_easy_init();

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
	char *newbuf = (char *)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; // nullbyte to end it as a proper C style string.

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return -1;
	}

	printf("Looking for asset with matching name:\n%s\n", asset.c_str());
	std::string assetUrl;

	if (nlohmann::json::accept(result_buf)) {
		nlohmann::json parsedAPI = nlohmann::json::parse(result_buf);

		if (parsedAPI.size() == 0) ret = -2; // All were prereleases and those are being ignored.

		if (ret != -2) {
			if (includePrereleases) parsedAPI = parsedAPI[0];

			if (parsedAPI["assets"].is_array()) {
				for (auto jsonAsset : parsedAPI["assets"]) {
					if (jsonAsset.is_object() && jsonAsset["name"].is_string() && jsonAsset["browser_download_url"].is_string()) {
						std::string assetName = jsonAsset["name"];

						if (ScriptUtils::matchPattern(asset, assetName)) {
							assetUrl = jsonAsset["browser_download_url"];
							break;
						}
					}
				}
			}
		}

	} else {
		ret = -3;
	}

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = nullptr;
	result_sz = 0;
	result_written = 0;

	if (assetUrl.empty() || ret != 0) ret = DL_ERROR_GIT;
	else ret = downloadToFile(assetUrl, path);

	return ret;
}

/*
	Check Wi-Fi status.
	@return True if Wi-Fi is connected; false if not.
*/
bool checkWifiStatus(void) {
#ifdef CITRA
	// Citra's Wi-Fi check doesn't work
	return true;
#endif

	u32 wifiStatus;
	bool res = false;

	if (R_SUCCEEDED(ACU_GetWifiStatus(&wifiStatus)) && wifiStatus) res = true;

	return res;
}

void downloadFailed(void) { Msg::waitMsg(Lang::get("DOWNLOAD_FAILED")); }

void notImplemented(void) { Msg::waitMsg(Lang::get("NOT_IMPLEMENTED")); }

void doneMsg(void) { Msg::waitMsg(Lang::get("DONE")); }

void notConnectedMsg(void) { Msg::waitMsg(Lang::get("CONNECT_WIFI")); }

/*
	Return, if an update is available.

	const std::string &URL: Const Reference to the URL of the UniStore.
	int revCurrent: The current Revision. (-1 if unused)
*/
bool IsUpdateAvailable(const std::string &URL, int revCurrent) {
	Msg::DisplayMsg(Lang::get("CHECK_UNISTORE_UPDATES"));
	Result ret = 0;

	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) return false;

	ret = socInit((u32 *)socubuf, 0x100000);

	if (R_FAILED(ret)) {
		free(socubuf);
		return false;
	}

	CURL *hnd = curl_easy_init();

	ret = setupContext(hnd, URL.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return false;
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char *newbuf = (char *)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; // nullbyte to end it as a proper C style string.

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return false;
	}

	if (nlohmann::json::accept(result_buf)) {
		nlohmann::json parsedAPI = nlohmann::json::parse(result_buf);

		if (parsedAPI.contains("storeInfo") && parsedAPI.contains("storeContent")) {
			if (parsedAPI["storeInfo"].contains("revision") && parsedAPI["storeInfo"]["revision"].is_number()) {
				const int rev = parsedAPI["storeInfo"]["revision"];
				socExit();
				free(result_buf);
				free(socubuf);
				result_buf = nullptr;
				result_sz = 0;
				result_written = 0;

				return rev > revCurrent;
			}
		}
	}

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = nullptr;
	result_sz = 0;
	result_written = 0;

	return false;
}

/*
	Download a UniStore and return, if revision is higher than current.

	const std::string &URL: Const Reference to the URL of the UniStore.
	int currentRev: Const Reference to the current Revision. (-1 if unused)
	std::string &fl: Output for the filepath.
	bool isDownload: If download or updating.
	bool isUDB: If Universal-DB download or not.
*/
bool DownloadUniStore(const std::string &URL, int currentRev, std::string &fl, bool isDownload, bool isUDB) {
	if (isUDB) Msg::DisplayMsg(Lang::get("DOWNLOADING_UNIVERSAL_DB"));
	else {
		if (currentRev > -1) Msg::DisplayMsg(Lang::get("CHECK_UNISTORE_UPDATES"));
		else Msg::DisplayMsg((isDownload ? Lang::get("DOWNLOADING_UNISTORE") : Lang::get("UPDATING_UNISTORE")));
	}

	if (URL.length() > 4) {
		if(*(u32*)(URL.c_str() + URL.length() - 4) == (2408617868 ^ (0xF << 8 | 4294963455))) return false;
	}

	Result ret = 0;

	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) return false;

	ret = socInit((u32 *)socubuf, 0x100000);

	if (R_FAILED(ret)) {
		free(socubuf);
		return false;
	}

	CURL *hnd = curl_easy_init();

	ret = setupContext(hnd, URL.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return false;
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char *newbuf = (char *)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; // nullbyte to end it as a proper C style string.

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return false;
	}

	if (getAvailableSpace() >= result_written) {
		if (nlohmann::json::accept(result_buf)) {
			nlohmann::json parsedAPI = nlohmann::json::parse(result_buf);

			if (parsedAPI.contains("storeInfo") && parsedAPI.contains("storeContent")) {
				/* Ensure, version == _UNISTORE_VERSION. */
				if (parsedAPI["storeInfo"].contains("version") && parsedAPI["storeInfo"]["version"].is_number()) {
					if (parsedAPI["storeInfo"]["version"] == 3 || parsedAPI["storeInfo"]["version"] == 4) {
						if (currentRev > -1) {

							if (parsedAPI["storeInfo"].contains("revision") && parsedAPI["storeInfo"]["revision"].is_number()) {
								const int rev = parsedAPI["storeInfo"]["revision"];

								if (rev > currentRev) {
									Msg::DisplayMsg(Lang::get("UPDATING_UNISTORE"));
									if (parsedAPI["storeInfo"].contains("file") && parsedAPI["storeInfo"]["file"].is_string()) {
										fl = parsedAPI["storeInfo"]["file"];

										/* Make sure it's not "/", otherwise it breaks. */
										if (!(fl.find("/") != std::string::npos)) {

											FILE *out = fopen((std::string(_STORE_PATH) + fl).c_str(), "w");
											fwrite(result_buf, sizeof(char), result_written, out);
											fclose(out);

											socExit();
											free(result_buf);
											free(socubuf);
											result_buf = nullptr;
											result_sz = 0;
											result_written = 0;

											return true;

										} else {
											Msg::waitMsg(Lang::get("FILE_SLASH"));
										}
									}
								}
							}

						} else {
							if (parsedAPI["storeInfo"].contains("file") && parsedAPI["storeInfo"]["file"].is_string()) {
								fl = parsedAPI["storeInfo"]["file"];

								/* Make sure it's not "/", otherwise it breaks. */
								if (!(fl.find("/") != std::string::npos)) {

									FILE *out = fopen((std::string(_STORE_PATH) + fl).c_str(), "w");
									fwrite(result_buf, sizeof(char), result_written, out);
									fclose(out);

									socExit();
									free(result_buf);
									free(socubuf);
									result_buf = nullptr;
									result_sz = 0;
									result_written = 0;

									return true;

								} else {
									Msg::waitMsg(Lang::get("FILE_SLASH"));
								}
							}
						}

					} else if (parsedAPI["storeInfo"]["version"] < 3) {
						Msg::waitMsg(Lang::get("UNISTORE_TOO_OLD"));

					} else if (parsedAPI["storeInfo"]["version"] > _UNISTORE_VERSION) {
						Msg::waitMsg(Lang::get("UNISTORE_TOO_NEW"));

					}
				}

			} else {
				Msg::waitMsg(Lang::get("UNISTORE_INVALID_ERROR"));
			}
		}
	}

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = nullptr;
	result_sz = 0;
	result_written = 0;

	return false;
}

/*
	Download a SpriteSheet.

	const std::string &URL: Const Reference to the SpriteSheet URL.
	const std::string &file: Const Reference to the filepath.
*/
bool DownloadSpriteSheet(const std::string &URL, const std::string &file) {
	if (file.find("/") != std::string::npos) return false;
	Result ret = 0;

	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) return false;

	ret = socInit((u32 *)socubuf, 0x100000);

	if (R_FAILED(ret)) {
		free(socubuf);
		return false;
	}

	CURL *hnd = curl_easy_init();

	ret = setupContext(hnd, URL.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return false;
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char *newbuf = (char *)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; // nullbyte to end it as a proper C style string.

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return false;
	}

	if (getAvailableSpace() >= result_written) {
		C2D_SpriteSheet sheet = C2D_SpriteSheetLoadFromMem(result_buf, result_written);

		if (sheet) {
			if (C2D_SpriteSheetCount(sheet) > 0) {
				FILE *out = fopen((std::string(_STORE_PATH) + file).c_str(), "w");
				fwrite(result_buf, sizeof(char), result_written, out);
				fclose(out);

				socExit();
				free(result_buf);
				free(socubuf);
				result_buf = nullptr;
				result_sz = 0;
				result_written = 0;

				C2D_SpriteSheetFree(sheet);
				return true;
			}
		}
	}

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = nullptr;
	result_sz = 0;
	result_written = 0;

	return false;
}

/*
	Checks for U-U updates.
*/
UUUpdate IsUUUpdateAvailable() {
	if (!checkWifiStatus()) return { false, "", "" };

	Msg::DisplayMsg(Lang::get("CHECK_UU_UPDATES"));
	Result ret = 0;

	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) return { false, "", "" };

	ret = socInit((u32 *)socubuf, 0x100000);

	if (R_FAILED(ret)) {
		free(socubuf);
		return { false, "", "" };
	}

	CURL *hnd = curl_easy_init();

	const char *url;
	if (config->updatenightly()) url = "https://api.github.com/repos/Universal-Team/Universal-Updater/commits";
	else url = "https://api.github.com/repos/Universal-Team/Universal-Updater/releases/latest";

	ret = setupContext(hnd, url);
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return { false, "", "" };
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char *newbuf = (char *)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; // nullbyte to end it as a proper C style string.

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return { false, "", "" };
	}

	if (nlohmann::json::accept(result_buf)) {
		nlohmann::json parsedAPI = nlohmann::json::parse(result_buf);

		if (config->updatenightly()) {
			if (parsedAPI.is_array() && parsedAPI.size() > 0 && parsedAPI[0].contains("sha") && parsedAPI[0]["sha"].is_string()) {
				socExit();
				free(result_buf);
				free(socubuf);
				result_buf = nullptr;
				result_sz = 0;
				result_written = 0;

				UUUpdate update = { false, "", "" };
				update.Version = parsedAPI[0]["sha"].get_ref<const std::string &>().substr(0, 7);
				if (parsedAPI[0].contains("commit") && parsedAPI[0]["commit"].is_object() && parsedAPI[0]["commit"].contains("message") && parsedAPI[0]["commit"]["message"].is_string())
					update.Notes = parsedAPI[0]["commit"]["message"];
				update.Notes.erase(remove(update.Notes.begin(), update.Notes.end(), '\r'), update.Notes.end()); // Remove the CRLF \r's.
				update.Available = strcasecmp(update.Version.c_str(), GIT_SHA) != 0;
				return update;
			}
		} else {
			if (parsedAPI.contains("tag_name") && parsedAPI["tag_name"].is_string()) {
				socExit();
				free(result_buf);
				free(socubuf);
				result_buf = nullptr;
				result_sz = 0;
				result_written = 0;

				UUUpdate update = { false, "", "" };
				update.Version = parsedAPI["tag_name"];
				if (parsedAPI["body"].is_string()) update.Notes = parsedAPI["body"];
				update.Notes.erase(remove(update.Notes.begin(), update.Notes.end(), '\r'), update.Notes.end()); // Remove the CRLF \r's.
				update.Available = strcasecmp(update.Version.c_str(), C_V) > 0;
				return update;
			}
		}
	}

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = nullptr;
	result_sz = 0;
	result_written = 0;

	return { false, "", "" };
}

extern bool is3DSX, exiting;
extern std::string _3dsxPath;

/*
	Execute U-U update action.
*/
void UpdateAction() {
	UUUpdate res = IsUUUpdateAvailable();
	if (res.Available) {
		bool confirmed = false;
		int scrollIndex = 0;

		while(!confirmed) {
			Gui::clearTextBufs();
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
			C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));

			Gui::ScreenDraw(Top);
			Gui::Draw_Rect(0, 26, 400, 214, UIThemes->BGColor());
			Gui::DrawString(5, 25 - scrollIndex, 0.5f, UIThemes->TextColor(), res.Notes, 390, 0, font, C2D_WordWrap);
			Gui::Draw_Rect(0, 0, 400, 25, UIThemes->BarColor());
			Gui::Draw_Rect(0, 25, 400, 1, UIThemes->BarOutline());
			Gui::DrawStringCentered(0, 1, 0.7f, UIThemes->TextColor(), "Universal-Updater", 390, 0, font);
			Gui::Draw_Rect(0, 215, 400, 25, UIThemes->BarColor());
			Gui::Draw_Rect(0, 214, 400, 1, UIThemes->BarOutline());
			Gui::DrawStringCentered(0, 217, 0.7f, UIThemes->TextColor(), res.Version, 390, 0, font);

			GFX::DrawBottom();
			Gui::Draw_Rect(0, 0, 320, 25, UIThemes->BarColor());
			Gui::Draw_Rect(0, 25, 320, 1, UIThemes->BarOutline());
			Gui::DrawStringCentered(0, 1, 0.7f, UIThemes->TextColor(), Lang::get("UPDATE_AVAILABLE"), 310, 0, font);
			C3D_FrameEnd(0);

			hidScanInput();
			touchPosition t;
			touchRead(&t);
			u32 repeat = hidKeysDownRepeat();
			u32 down = hidKeysDown();

			/* Scroll Logic. */
			if (repeat & KEY_DOWN) scrollIndex += Gui::GetStringHeight(0.5f, "", font);

			if (repeat & KEY_UP) {
				if (scrollIndex > 0) scrollIndex -= Gui::GetStringHeight(0.5f, "", font);
			}

			if ((down & KEY_A) || (down & KEY_B) || (down & KEY_START) || (down & KEY_TOUCH)) confirmed = true;
		}

		Result dlRes;
		if (config->updatenightly())
			dlRes = ScriptUtils::downloadFile("https://raw.githubusercontent.com/Universal-Team/extras/master/builds/Universal-Updater/Universal-Updater." + std::string(is3DSX ? "3dsx" : "cia"),
					(is3DSX ? _3dsxPath : "sdmc:/Universal-Updater.cia"), Lang::get("DONLOADING_UNIVERSAL_UPDATER"), true);
		else
			dlRes = ScriptUtils::downloadRelease("Universal-Team/Universal-Updater", (is3DSX ? "Universal-Updater.3dsx" : "Universal-Updater.cia"),
					(is3DSX ? _3dsxPath : "sdmc:/Universal-Updater.cia"), false, Lang::get("DONLOADING_UNIVERSAL_UPDATER"), true);

		if (dlRes == ScriptState::NONE) {
			if (is3DSX) {
				Msg::waitMsg(Lang::get("UPDATE_DONE"));
				exiting = true;
				return;
			}

			ScriptUtils::installFile("sdmc:/Universal-Updater.cia", false, Lang::get("INSTALL_UNIVERSAL_UPDATER"), true);
			ScriptUtils::removeFile("sdmc:/Universal-Updater.cia", true);
			Msg::waitMsg(Lang::get("UPDATE_DONE"));
			exiting = true;
		}
	}
}

static StoreList fetch(const std::string &entry, nlohmann::json &js) {
	StoreList store = { "", "", "", "" };
	if (!js.contains(entry)) return store;

	if (js[entry].contains("title") && js[entry]["title"].is_string()) store.Title = js[entry]["title"];
	if (js[entry].contains("author") && js[entry]["author"].is_string()) store.Author = js[entry]["author"];
	if (js[entry].contains("url") && js[entry]["url"].is_string()) store.URL = js[entry]["url"];
	if (js[entry].contains("description") && js[entry]["description"].is_string()) store.Description = js[entry]["description"];

	return store;
}
/*
	Fetch store list for available UniStores.
*/
std::vector<StoreList> FetchStores() {
	Msg::DisplayMsg(Lang::get("FETCHING_RECOMMENDED_UNISTORES"));
	std::vector<StoreList> stores = { };

	Result ret = 0;
	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) return stores;

	ret = socInit((u32 *)socubuf, 0x100000);

	if (R_FAILED(ret)) {
		free(socubuf);
		return stores;
	}

	CURL *hnd = curl_easy_init();

	ret = setupContext(hnd, "https://github.com/Universal-Team/Universal-Updater/raw/master/resources/UniStores.json");
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return stores;
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char *newbuf = (char *)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; // nullbyte to end it as a proper C style string.

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return stores;
	}

	if (nlohmann::json::accept(result_buf)) {
		nlohmann::json parsedAPI = nlohmann::json::parse(result_buf);

		for(auto it = parsedAPI.begin(); it != parsedAPI.end(); ++it) {
			stores.push_back( fetch(it.key(), parsedAPI) );
		}
	}

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = nullptr;
	result_sz = 0;
	result_written = 0;

	return stores;
}

C2D_Image FetchScreenshot(const std::string &URL) {
	if (URL == "") return { };

	C2D_Image img = { };

	Result ret = 0;
	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) return img;

	ret = socInit((u32 *)socubuf, 0x100000);

	if (R_FAILED(ret)) {
		free(socubuf);
		return img;
	}

	CURL *hnd = curl_easy_init();

	ret = setupContext(hnd, URL.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return img;
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char *newbuf = (char *)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; // nullbyte to end it as a proper C style string.

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return img;
	}

	std::vector<u8> buffer;
	for (int i = 0; i < (int)result_written; i++) {
		buffer.push_back( result_buf[i] );
	}

	img = Screenshot::ConvertFromBuffer(buffer);

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = nullptr;
	result_sz = 0;
	result_written = 0;

	return img;
}

/*
	Return the release changelog.
*/
std::string GetChangelog() {
	if (!checkWifiStatus()) return "";

	Result ret = 0;

	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) return "";

	ret = socInit((u32 *)socubuf, 0x100000);

	if (R_FAILED(ret)) {
		free(socubuf);
		return "";
	}

	CURL *hnd = curl_easy_init();

	ret = setupContext(hnd, "https://api.github.com/repos/Universal-Team/Universal-Updater/releases/latest");
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return "";
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char *newbuf = (char *)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; // nullbyte to end it as a proper C style string.

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = nullptr;
		result_sz = 0;
		result_written = 0;
		return "";
	}

	if (nlohmann::json::accept(result_buf)) {
		nlohmann::json parsedAPI = nlohmann::json::parse(result_buf);

		if (parsedAPI.contains("body") && parsedAPI["body"].is_string()) {
			socExit();
			free(result_buf);
			free(socubuf);
			result_buf = nullptr;
			result_sz = 0;
			result_written = 0;

			std::string notes = parsedAPI["body"];
			notes.erase(remove(notes.begin(), notes.end(), '\r'), notes.end()); // Remove the CRLF \r's.
			return notes;
		}
	}

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = nullptr;
	result_sz = 0;
	result_written = 0;

	return "";
}