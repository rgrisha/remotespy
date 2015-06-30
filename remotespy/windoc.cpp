
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
	SetLastError(ERROR_SUCCESS);
	::GetWindowText(hwnd, buffer, sizeof(buffer));
	if (GetLastError() == ERROR_SUCCESS) {
		utility::string_t window_text(buffer);
		lwe[_T("text")] = json::value::string(window_text);	
	}
	else {
		uclog << _T("Error in doc, GetWindowText: ") << GetLastErrorAsString() << endl;
	}

	buffer[0] = 0;
	SetLastError(ERROR_SUCCESS);
	::GetClassName(hwnd, buffer, sizeof(buffer));
	if (GetLastError() == ERROR_SUCCESS) {
		utility::string_t window_class(buffer);
		lwe[_T("wclass")] = json::value::string(window_class);
	}
	else {
		uclog << _T("Error in doc, GetClassName: ") << GetLastErrorAsString() << endl;
	}
	

	DWORD process_id;
	GetWindowThreadProcessId(hwnd, &process_id);
	
	HANDLE Handle = OpenProcess(PROCESS_QUERY_INFORMATION , FALSE, process_id);
	if (Handle) {
		TCHAR buffproc[MAX_PATH];
		DWORD psize = sizeof(buffproc);

		buffproc[0] = 0;
		if (QueryFullProcessImageName(Handle, 0, buffproc, &psize)){
			lwe[_T("process")] = json::value::string(buffproc);
		}
		CloseHandle(Handle);
	}

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