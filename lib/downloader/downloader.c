/*
 * MIT License
 *
 * Copyright (c) 2022 Aditya
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>

#include "downloader.h"

int IsCurlAvailable = 0;
int IsWgetAvailable = 0;

#define DownloaderCmdSize 2048

#if defined(WIN32) || defined(_WIN32)
	#define _downloader_surpresser ">nul 2>nul"
#else
	#define _downloader_surpresser "> /dev/null 2>&1"
#endif

int DownloaderCheckBackends() {
	char cmd[512] = "";

	snprintf(cmd, 512, "curl --version %s", _downloader_surpresser);
	int curlversion_cmd = system(cmd);

	snprintf(cmd, 512, "wget --version %s", _downloader_surpresser);
	int wgetversion_cmd = system(cmd);

	if (curlversion_cmd == 0) {
		IsCurlAvailable = 1;
		return 0;
	}
	if (wgetversion_cmd == 0) {
		IsWgetAvailable = 1;
		return 0;
	}

	return -1;
}

int DownloadFileFrom(const char* url, const char* filePath) {
	char command[DownloaderCmdSize] = "";

	if (IsCurlAvailable == 1) {
		snprintf(command, DownloaderCmdSize, "curl -L %s --output %s %s", url, filePath, _downloader_surpresser);
		system((const char*)command);
		return 0;
	} else if (IsWgetAvailable == 1) {
		snprintf(command, DownloaderCmdSize, "wget -O %s %s %s", filePath, url, _downloader_surpresser);
		system((const char*)command);
		return 0;
	}

	return -1;
}
