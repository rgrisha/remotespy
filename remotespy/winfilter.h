
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

	void FilterWindowRightOf(int left);
	void FilterWindowLeftOf(int right);
	void FilterWindowBelowOf(int top);
	void FilterWindowAboveOf(int bottom);

	void FilterWindowPid(int pid);
	void FilterWindowParent(int parent);
	void FilterWindowId(long id);

	void FilterWindowProcessName(utility::string_t text);

	BOOL ApplyFilter(HWND hWnd);

};

#endif //#ifndef WINFILTER_HEADER