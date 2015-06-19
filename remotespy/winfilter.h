
#ifndef WINFILTER_HEADER
#define WINFILTER_HEADER

#include "stdafx.h"

class WindowsFilter {

private:
	static BOOL CALLBACK WinCallbackProc(HWND hWnd, LPARAM lParam);
	std::vector<std::function<BOOL(HWND hWnd)>> filters;

public:
	WindowsDocument* windows_document;

	WindowsFilter();
	~WindowsFilter();

	WindowsDocument* CreateWindowsDocumentForMainWindows();
	WindowsDocument* CreateWindowsDocumentForChildWindows(HWND hWnd);
	WindowsDocument* CreateWindowsDocument();

	void FilterWindowText(utility::string_t text);
	void FilterWindowClass(utility::string_t text);
	void FilterWindowInsideRect(RECT rect);

	BOOL ApplyFilter(HWND hWnd);

};

#endif //#ifndef WINFILTER_HEADER