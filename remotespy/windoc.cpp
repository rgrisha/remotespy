
#include "stdafx.h"

WindowsDocument::WindowsDocument() {
}

WindowsDocument::~WindowsDocument() {
}

void WindowsDocument::CreateWindowEntry(HWND hwnd) {

	json::value lwe;
	TCHAR buffer[1024];
	buffer[0] = 0;
	
	lwe[_T("hwnd")] = json::value::number((int)hwnd);
	::GetWindowText(hwnd, buffer, sizeof(buffer));
	utility::string_t window_text(buffer);
	lwe[_T("text")] = json::value::string(window_text);

	buffer[0] = 0;
	::GetClassName(hwnd, buffer, sizeof(buffer));
	utility::string_t window_class(buffer);
	lwe[_T("wclass")] = json::value::string(window_class);

	DWORD process_id;
	GetWindowThreadProcessId(hwnd, &process_id);
	
	RECT rect;
	if (GetWindowRect(hwnd, &rect)) {
		lwe[_T("left")] = json::value::number(rect.left);
		lwe[_T("right")] = json::value::number(rect.right);
		lwe[_T("top")] = json::value::number(rect.top);
		lwe[_T("bottom")] = json::value::number(rect.bottom);
	}
	lwe[_T("pid")] = json::value::number((int)process_id);
	lwe[_T("parent")] = json::value::number((int)GetParent(hwnd));
	lwe[_T("style_ex")] = json::value::number(GetWindowLong(hwnd, GWL_EXSTYLE));
	lwe[_T("hinstance")] = json::value::number(GetWindowLong(hwnd, GWL_HINSTANCE));
	lwe[_T("id")] = json::value::number(GetWindowLong(hwnd, GWL_ID ));
	lwe[_T("style")] = json::value::number(GetWindowLong(hwnd, GWL_STYLE ));

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