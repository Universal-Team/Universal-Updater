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

#include "Downloading.hpp"

#include <string.h>

#define USER_AGENT "Universal-Updater-v4.0.0"

static size_t Result_Size = 0;
static size_t Result_Written = 0;
static FILE *Out = nullptr;


static size_t handle_data(char *Ptr, size_t Size, size_t NMemb, void *UserData) {
	Result_Written += Size * NMemb;
	iprintf("handle_data - %d\n", Result_Written);

	if (!Out) return 0;

	return fwrite(Ptr, Size, NMemb, Out) * Size;
};

static int setupContext(CURL *Hnd, const char * url) {
	curl_easy_setopt(Hnd, CURLOPT_BUFFERSIZE, 102400L);
	curl_easy_setopt(Hnd, CURLOPT_URL, url);
	curl_easy_setopt(Hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(Hnd, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(Hnd, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(Hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(Hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(Hnd, CURLOPT_WRITEFUNCTION, handle_data);
	curl_easy_setopt(Hnd, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(Hnd, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(Hnd, CURLOPT_STDERR, stdout);

	return 0;
};

int downloadToFile(const std::string &URL, const std::string &Path) {
	int Ret = 0;

	CURL *Hnd = curl_easy_init();
	Ret = setupContext(Hnd, URL.c_str());
	if (Ret != 0) {
		Result_Size = 0;
		Result_Written = 0;
		return Ret;
	}

	Out = fopen(Path.c_str(), "wb");
	if(!Out) {
		Result_Size = 0;
		Result_Written = 0;
		return -2;
	}

	CURLcode CRes = curl_easy_perform(Hnd);
	curl_easy_cleanup(Hnd);

	if (CRes != CURLE_OK) {
		printf("Error in:\ncurl\n");
		fclose(Out);
		Result_Size = 0;
		Result_Written = 0;
		return -1;
	}

	fclose(Out);
	Result_Size = 0;
	Result_Written = 0;

	return 0;
};