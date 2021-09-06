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

#include "DownloadUtils.hpp"
#include <algorithm>
#include <cstring>

#ifdef _3DS
	#include <3ds.h> // For socInit stuff.
	#include <malloc.h> // memalign and free.
	#define RESULT_BUF_SIZE (1 << 20) // 1 MiB.

#elif ARM9
	#include "WiFi.hpp"
	#define RESULT_BUF_SIZE (1 << 20) // 1 MiB.
#endif

#define USER_AGENT "Universal-Updater-v4.0.0"


static size_t ResultWritten = 0;

static char *ResultBuf = nullptr;
static size_t BufWritten = 0;
static size_t BufSize = 0;

static FILE *Out = nullptr;

static CURL *Hnd = nullptr;
curl_off_t DownloadTotal = 1; // Dont initialize with 0 to avoid division by zero later.
curl_off_t DownloadNow = 0;


static size_t HandleData(const char *Ptr, size_t Size, size_t NMemb, const void *UserData) {
	const size_t RealSize = Size * NMemb;
	ResultWritten += RealSize;

	if (BufWritten + RealSize > RESULT_BUF_SIZE) {
		if (!Out) return 0;
		const size_t Ret = fwrite(ResultBuf, 1LLU, BufWritten, Out);
		if (Ret != BufWritten) return Ret;
		BufWritten = 0;
	}

	if (!ResultBuf) return 0;
	memcpy(ResultBuf + BufWritten, Ptr, RealSize);
	BufWritten += RealSize;

	return RealSize;
};

static int HandleProgress(CURL *hnd, curl_off_t DlTotal, curl_off_t DlNow, curl_off_t UlTotal, curl_off_t UlNow) {
	DownloadTotal = DlTotal;
	DownloadNow = DlNow;

	return 0;
}


static int SetupContext(CURL *Hnd, const char *URL) {
	curl_easy_setopt(Hnd, CURLOPT_BUFFERSIZE, 102400L);
	curl_easy_setopt(Hnd, CURLOPT_URL, URL);
	curl_easy_setopt(Hnd, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(Hnd, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(Hnd, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(Hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(Hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(Hnd, CURLOPT_WRITEFUNCTION, HandleData);
	curl_easy_setopt(Hnd, CURLOPT_XFERINFOFUNCTION, HandleProgress);
	curl_easy_setopt(Hnd, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(Hnd, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(Hnd, CURLOPT_STDERR, stdout);

	return 0;
};


int DownloadUtils::DownloadToFile(const char *URL, const char *Path) {
	if (!DownloadUtils::WiFiAvailable()) return -1;

	int Ret = 0;
	CURLcode CRes;

	#ifdef _3DS
		bool SocInitialized = false;

		void *SocBuf = memalign(0x1000, 0x100000);
		if (!SocBuf) {
			Ret = -1;
			goto Cleanup;
		}

		Ret = socInit((uint32_t *)SocBuf, 0x100000);
		if (R_FAILED(Ret)) {
			goto Cleanup;
		}

		SocInitialized = true;
	#endif

	BufSize = RESULT_BUF_SIZE;
	ResultBuf = new char[BufSize];
	if (!ResultBuf) {
		Ret = -1;
		goto Cleanup;
	}

	Hnd = curl_easy_init();
	Ret = SetupContext(Hnd, URL);
	if (Ret != 0) {
		goto Cleanup;
	}

	Out = fopen(Path, "wb");
	if (!Out) {
		Ret = -2;
		goto Cleanup;
	}

	CRes = curl_easy_perform(Hnd);
	curl_easy_cleanup(Hnd);
	Hnd = nullptr;
	ResultBuf[ResultWritten] = 0;
	
	if (CRes != CURLE_OK) {
		printf("Error in:\ncurl\n");
		Ret = -3;
		goto Cleanup;
	}

	if (BufWritten > 0) {
		fwrite(ResultBuf, 1, BufWritten, Out);
	}

Cleanup:
	#ifdef _3DS
		if (SocInitialized) socExit();
		if (SocBuf) free(SocBuf);
	#endif

	/* If file is open -> Close it. */
	if (Out) {
		fclose(Out);
		Out = nullptr;
	}

	/* If the ResultBuf is used -> delete it. */
	if (ResultBuf) {
		delete[] ResultBuf;
		ResultBuf = nullptr;
	}

	if (Hnd) {
		curl_easy_cleanup(Hnd);
		Hnd = nullptr;
	}

	size_t FinalSize = ResultWritten;

	/* Reset some sizes. */
	ResultWritten = 0, BufSize = 0, BufWritten = 0, DownloadTotal = 1, DownloadNow = 0;

	return FinalSize;
};


int DownloadUtils::DownloadToMemory(const char *URL, void *Buffer, const size_t Size) {
	if (!DownloadUtils::WiFiAvailable()) return -1;

	int Ret = 0;
	CURLcode CRes;

	BufSize = Size;
	ResultBuf = (char *)Buffer;

	#ifdef _3DS
		bool SocInitialized = false;

		void *SocBuf = memalign(0x1000, 0x100000);
		if (!SocBuf) {
			Ret = -1;
			goto Cleanup;
		}

		Ret = socInit((uint32_t *)SocBuf, 0x100000);
		if (R_FAILED(Ret)) {
			goto Cleanup;
		}

		SocInitialized = true;
	#endif

	Hnd = curl_easy_init();
	Ret = SetupContext(Hnd, URL);
	if (Ret != 0) {
		goto Cleanup;
	}

	CRes = curl_easy_perform(Hnd);
	curl_easy_cleanup(Hnd);
	Hnd = nullptr;
	ResultBuf[ResultWritten] = 0;

	if (CRes != CURLE_OK) {
		printf("Error in:\ncurl\n");
		Ret = -3;
		goto Cleanup;
	}

Cleanup:
	#ifdef _3DS
		if (SocInitialized) socExit();
		if (SocBuf) free(SocBuf);
	#endif

	if (Hnd) {
		curl_easy_cleanup(Hnd);
		Hnd = nullptr;
	}

	size_t FinalSize = ResultWritten;

	/* Reset some sizes. */
	ResultWritten = 0, BufSize = 0, BufWritten = 0, DownloadTotal = 1, DownloadNow = 0;
	ResultBuf = nullptr;

	return FinalSize;
};


curl_off_t DownloadUtils::Speed() {
	curl_off_t downloadSpeed = 0;
	if (Hnd) curl_easy_getinfo(Hnd, CURLINFO_SPEED_DOWNLOAD_T, &downloadSpeed);

	return downloadSpeed;
}

curl_off_t DownloadUtils::CurrentProgress() { return DownloadNow; }


/* std::max with 1 so as never to divide by 0 with this. */
curl_off_t DownloadUtils::TotalSize() { return std::max(1LL, DownloadTotal); }


bool DownloadUtils::WiFiAvailable() {
	#ifdef _3DS
		// return true; // For Citra

		uint32_t WifiStatus;
		return (R_SUCCEEDED(ACU_GetWifiStatus(&WifiStatus)) && WifiStatus);

	#elif ARM9
		return WiFi::Connected();
	#endif
};