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

#ifdef _3DS
#define RESULT_BUF_SIZE 1 << 30 // 1 MiB
#elif ARM9
#define RESULT_BUF_SIZE 1 << 30 // 1 MiB
#endif

#define USER_AGENT "Universal-Updater-v4.0.0"

static size_t ResultSize = 0;
static size_t ResultWritten = 0;

static char *ResultBuf = nullptr;
static size_t BufWritten = 0;
static size_t BufSize = 0;

static FILE *Out = nullptr;


static size_t handle_data(const char *Ptr, size_t Size, size_t NMemb, const void *UserData) {
	size_t RealSize = Size * NMemb;
	ResultWritten += RealSize;

	if (BufWritten + RealSize > RESULT_BUF_SIZE) {
		if (!Out) return 0;
		size_t ret = fwrite(ResultBuf, 1, BufWritten, Out);
		if (ret != BufWritten) return ret;
		BufWritten = 0;
	}

	if (!ResultBuf) return 0;
	memcpy(ResultBuf + BufWritten, Ptr, RealSize);
	BufWritten += RealSize;
	return RealSize;
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

int downloadToFile(const char *URL, const char *Path) {
	int Ret = 0;

	BufSize = RESULT_BUF_SIZE;
	ResultBuf = new char[BufSize];
	if (!ResultBuf) {
		BufSize = 0;
		ResultSize = 0;
		ResultWritten = 0;
		return -1;
	}


	CURL *Hnd = curl_easy_init();
	Ret = setupContext(Hnd, URL);
	if (Ret != 0) {
		delete[] ResultBuf;
		ResultBuf = nullptr;
		BufSize = 0;
		ResultSize = 0;
		ResultWritten = 0;
		return Ret;
	}

	Out = fopen(Path, "wb");
	if(!Out) {
		delete[] ResultBuf;
		ResultBuf = nullptr;
		BufSize = 0;
		ResultSize = 0;
		ResultWritten = 0;
		return -2;
	}

	CURLcode CRes = curl_easy_perform(Hnd);
	curl_easy_cleanup(Hnd);

	if (CRes != CURLE_OK) {
		printf("Error in:\ncurl\n");
		delete[] ResultBuf;
		ResultBuf = nullptr;
		fclose(Out);
		BufSize = 0;
		ResultSize = 0;
		ResultWritten = 0;
		return -3;
	}

	if (BufWritten > 0) {
		fwrite(ResultBuf, 1, BufWritten, Out);
	}

	delete[] ResultBuf;
	ResultBuf = nullptr;
	fclose(Out);
	BufSize = 0;
	ResultSize = 0;
	ResultWritten = 0;

	return 0;
};

int downloadToMemory(const char *URL, void *Buffer, size_t Size) {
	int Ret = 0;

	BufSize = Size;
	ResultBuf = (char *)Buffer;

	CURL *Hnd = curl_easy_init();
	Ret = setupContext(Hnd, URL);
	if (Ret != 0) {
		BufSize = 0;
		ResultSize = 0;
		ResultWritten = 0;
		return Ret;
	}

	CURLcode CRes = curl_easy_perform(Hnd);
	curl_easy_cleanup(Hnd);

	if (CRes != CURLE_OK) {
		printf("Error in:\ncurl\n");
		fclose(Out);
		BufSize = 0;
		ResultSize = 0;
		ResultWritten = 0;
		return -3;
	}

	fclose(Out);
	BufSize = 0;
	ResultSize = 0;
	ResultWritten = 0;

	return 0;
};
