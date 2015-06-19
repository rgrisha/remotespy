
#include "stdafx.h"

WindowsDocument::WindowsDocument() {
}

WindowsDocument::~WindowsDocument() {
}

void WindowsDocument::CreateWindowEntry(HWND hwnd) {

	json::value lwe;

	TCHAR buffer[1024];
	lwe[L"hwnd"] = json::value::number((int)hwnd);
	::GetWindowText(hwnd, buffer, sizeof(buffer));
	lwe[L"text"] = json::value::string(buffer);

	TCHAR buffer1[1024];
	::GetClassName(hwnd, buffer1, sizeof(buffer1));
	lwe[L"class"] = json::value::string(buffer1);

	RECT rect;
	if (GetWindowRect(hwnd, &rect)) {
		lwe[L"left"] = json::value::number(rect.left);
		lwe[L"right"] = json::value::number(rect.right);
		lwe[L"top"] = json::value::number(rect.top);
		lwe[L"bottom"] = json::value::number(rect.bottom);
	}

	windows[windows_count++] = lwe;
}

json::value WindowsDocument::GetWindowsDoc() {
	return windows;
}

json::value WindowsDocument::GetWindowsDocDesktop() {
	HWND hWnd = ::GetDesktopWindow();
	CreateWindowEntry(hWnd);
	return windows;
}