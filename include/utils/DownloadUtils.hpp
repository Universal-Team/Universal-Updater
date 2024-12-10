// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_DOWNLOAD_UTILS_HPP
#define _UNIVERSAL_UPDATER_DOWNLOAD_UTILS_HPP

#include <curl/curl.h>

namespace DownloadUtils {
	int DownloadToFile(const char *URL, const char *Path);
	int DownloadToMemory(const char *URL, void *Buffer, const size_t Size);

	curl_off_t Speed();
	curl_off_t CurrentProgress();
	curl_off_t TotalSize();
};

#endif