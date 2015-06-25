
#include "stdafx.h"

WindowsFilter::WindowsFilter() {
	windows_document = NULL;
}

WindowsFilter::~WindowsFilter() {
	if (windows_document != NULL) delete windows_document; 
}

WindowsDocument* WindowsFilter::CreateWindowsDocument() {
	if (windows_document != NULL) delete windows_document; 
	windows_document = new WindowsDocument();
	return windows_document;
}

WindowsDocument* WindowsFilter::CreateWindowsDocumentForMainWindows() {
	if (windows_document != NULL) delete windows_document; 
	windows_document = new WindowsDocument();
	EnumWindows(WinCallbackProc, (LPARAM)this);
	return windows_document;
}

WindowsDocument* WindowsFilter::CreateWindowsDocumentForChildWindows(HWND hWnd) {
	if (windows_document != NULL) delete windows_document; 
	windows_document = new WindowsDocument();
	EnumChildWindows(hWnd, WinCallbackProc, (LPARAM)this);
	return windows_document;
}

BOOL CALLBACK WindowsFilter::WinCallbackProc(HWND hWnd, LPARAM lParam) {

	WindowsFilter* windows_filter = (WindowsFilter*)lParam;

	if (windows_filter->ApplyFilter(hWnd)) {
		windows_filter->windows_document->CreateWindowEntry(hWnd);
	}

	return TRUE;
}

BOOL WindowsFilter::ApplyFilter(HWND hWnd) {
	if (filters.empty()) return true;
	for (auto flt = filters.begin(), end = filters.end(); flt != end; flt++) {
		std::function<BOOL(HWND)> &fn = *flt;
		if (!fn(hWnd)) return false;
	}
	return true;
}

void WindowsFilter::FilterWindowText(utility::string_t text) {
	auto fn = [text](HWND hWnd) {
		TCHAR buffer[4096];
		buffer[0] = '\0';
		int ret = ::GetWindowText(hWnd, buffer, sizeof(buffer));
		if (ret == 0) {
			uclog << _T("Error in FilterWindowText: ") << GetLastErrorAsString() << endl;
			return false;
		}
		return (_tcsstr(buffer, text.c_str()) != NULL);
	};
	filters.push_back(fn);
}

void WindowsFilter::FilterWindowClass(utility::string_t text) {
	auto fn = [text](HWND hWnd) {
		TCHAR buffer[4096];
		buffer[0] = '\0';
		int ret = ::GetClassName(hWnd, buffer, sizeof(buffer));
		if (ret == 0) {
			uclog << _T("Error in FilterWindowClass: ") << GetLastErrorAsString() << endl;
			return false;
		}
		TCHAR* result = _tcsstr(buffer, text.c_str());
		return (result != NULL);
	};
	filters.push_back(fn);
}

void WindowsFilter::FilterWindowInsideRect(RECT rect) {
	auto fn = [&rect](HWND hWnd) {
		RECT rectw;
		
		if (GetWindowRect(hWnd, &rectw)) {
			return rect.top <= rectw.top && rect.left <= rectw.left &&
				rect.bottom >= rectw.bottom && rect.right >= rectw.right;
		} else {
			return false;
		}
	};
	filters.push_back(fn);
}