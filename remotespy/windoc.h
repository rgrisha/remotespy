
#ifndef WINDOC_HEADER
#define WINDOC_HEADER

#include "stdafx.h"

using namespace std;
using namespace web;

class WindowsDocument {
public:
	WindowsDocument();
	~WindowsDocument();

	void CreateWindowEntry(HWND hwnd);

	json::value GetWindowsDoc();
	json::value GetWindowsDocDesktop();

private:
	json::value windows;
	int windows_count = 0;

};

#endif //#ifndef WINDOC_HEADER