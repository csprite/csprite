#ifdef _WIN32

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tinyfiledialogs.h"

#ifdef _MSC_VER
#pragma warning(disable:4996) /* silences warning about wcscpy*/
#endif

/* WINDOWS ONLY */
int main(void) {
	wchar_t const * lPassword;
	wchar_t const * lTheSaveFileName;
	wchar_t const * lTheOpenFileName;
	wchar_t const * lTheSelectFolderName;
	wchar_t const * lTheHexColor;
	wchar_t const * lWillBeGraphicMode;
	unsigned char lRgbColor[3];
	FILE * lIn;
	wchar_t lWcharBuff[1024];
	wchar_t lBuffer[1024];
	wchar_t const * lFilterPatterns[2] = { L"*.txt", L"*.text" };

	lWillBeGraphicMode = tinyfd_inputBoxW(L"tinyfd_query", NULL, NULL);

	wcscpy(lBuffer, L"v");
	mbstowcs(lWcharBuff, tinyfd_version, strlen(tinyfd_version) + 1);
	wcscat(lBuffer, lWcharBuff);
	if (lWillBeGraphicMode)
	{
		wcscat(lBuffer, L"\ngraphic mode: ");
	}
	else
	{
		wcscat(lBuffer, L"\nconsole mode: ");
	}
	mbstowcs(lWcharBuff, tinyfd_response, strlen(tinyfd_response)+1);
	wcscat(lBuffer, lWcharBuff);
	wcscat(lBuffer, L"\n");
	mbstowcs(lWcharBuff, tinyfd_needs + 78, strlen(tinyfd_needs + 78) + 1);
	wcscat(lBuffer, lWcharBuff);

	tinyfd_messageBoxW(L"hello", lBuffer, L"ok", L"info", 0);

	tinyfd_notifyPopupW(L"the title", L"the message\n\tfrom outer-space", L"info");

	lPassword = tinyfd_inputBoxW(
		L"a password box", L"your password will be revealed later", NULL);

	if (!lPassword) return 1;

	lTheSaveFileName = tinyfd_saveFileDialogW(
		L"let us save this password",
		L"passwordFile.txt",
		2,
		lFilterPatterns,
		NULL);

	if (! lTheSaveFileName)
	{
		tinyfd_messageBoxW(
			L"Error",
			L"Save file name is NULL",
			L"ok",
			L"error",
			1);
		return 1 ;
	}

	lIn = _wfopen(lTheSaveFileName, L"wt, ccs=UNICODE");
	if (!lIn)
	{
		tinyfd_messageBoxW(
			L"Error",
			L"Can not open this file in write mode",
			L"ok",
			L"error",
			1);
		return 1 ;
	}
	fputws(lPassword, lIn);
	fclose(lIn);

	lTheOpenFileName = tinyfd_openFileDialogW(
		L"let us read the password back",
		L"",
		2,
		lFilterPatterns,
		NULL,
		0);

	if (! lTheOpenFileName)
	{
		tinyfd_messageBoxW(
			L"Error",
			L"Open file name is NULL",
			L"ok",
			L"error",
			1);
		return 1 ;
	}

	lIn = _wfopen(lTheOpenFileName, L"rt, ccs=UNICODE");

	if (!lIn)
	{
		tinyfd_messageBoxW(
			L"Error",
			L"Can not open this file in read mode",
			L"ok",
			L"error",
			1);
		return(1);
	}
	lBuffer[0] = '\0';
	fgetws(lBuffer, sizeof(lBuffer), lIn);
	fclose(lIn);

	tinyfd_messageBoxW(L"your password is",
			lBuffer, L"ok", L"info", 1);

	lTheSelectFolderName = tinyfd_selectFolderDialogW(
		L"let us just select a directory", L"C:\\");

	if (!lTheSelectFolderName)
	{
		tinyfd_messageBoxW(
			L"Error",
			L"Select folder name is NULL",
			L"ok",
			L"error",
			1);
		return 1;
	}

	tinyfd_messageBoxW(L"The selected folder is",
		lTheSelectFolderName, L"ok", L"info", 1);

	lTheHexColor = tinyfd_colorChooserW(
		L"choose a nice color",
		L"#FF0077",
		lRgbColor,
		lRgbColor);

	if (!lTheHexColor)
	{
		tinyfd_messageBoxW(
			L"Error",
			L"hexcolor is NULL",
			L"ok",
			L"error",
			1);
		return 1;
	}

	tinyfd_messageBoxW(L"The selected hexcolor is",
		lTheHexColor, L"ok", L"info", 1);

	tinyfd_messageBoxW(L"your password was", lPassword, L"ok", L"info", 1);

	tinyfd_beep();

	return 0;
}

#ifdef _MSC_VER
#pragma warning(default:4996)
#endif

#else

#include <stdio.h>

int main(void) {
	printf("The program was not supposed to be compiled on non-windows platforms\n");

	return 1;
}

#endif // _WIN32


/*
MinGW needs gcc >= v4.9 otherwise some headers are incomplete
> gcc -o hello.exe hello.c tinyfiledialogs.c -LC:/mingw/lib -lcomdlg32 -lole32

TinyCC needs >= v0.9.27 (+ tweaks - contact me) otherwise some headers are missing
> tcc -o hello.exe hello.c tinyfiledialogs.c ^
	-isystem C:\tcc\winapi-full-for-0.9.27\include\winapi ^
	-lcomdlg32 -lole32 -luser32 -lshell32

Borland C: > bcc32c -o hello.exe hello.c tinyfiledialogs.c
OpenWatcom v2: create a character-mode executable project.

VisualStudio :
	Create a console application project,
	it links against comdlg32.lib & ole32.lib.

VisualStudio command line :
	> cl hello.c tinyfiledialogs.c comdlg32.lib ole32.lib user32.lib shell32.lib /W4
*/
