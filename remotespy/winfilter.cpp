
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
		TCHAR buffer[1024];
		buffer[0] = '\0';
		SetLastError(ERROR_SUCCESS);
		::GetWindowText(hWnd, buffer, sizeof(buffer));
		if (GetLastError() != ERROR_SUCCESS) {
			uclog << _T("Error in FilterWindowText: ") << GetLastErrorAsString() << endl;
			return false;
		}
		return (_tcsstr(buffer, text.c_str()) != NULL);
	};
	filters.push_back(fn);
}

void WindowsFilter::FilterWindowClass(utility::string_t text) {
	auto fn = [text](HWND hWnd) {
		TCHAR buffer[1024];
		buffer[0] = '\0';
		SetLastError(ERROR_SUCCESS);
		::GetClassName(hWnd, buffer, sizeof(buffer));
		if (GetLastError() != ERROR_SUCCESS) {
			uclog << _T("Error in FilterWindowClass: ") << GetLastErrorAsString() << endl;
			return false;
		}
		TCHAR* result = _tcsstr(buffer, text.c_str());
		return (result != NULL);
	};
	filters.push_back(fn);
}

void WindowsFilter::FilterWindowRightOf(int left) {
	auto fn = [left](HWND hWnd) {
		RECT rectw;

		if (GetWindowRect(hWnd, &rectw)) {
			return rectw.left >= left;
		}
		else {
			return false;
		}
	};
	filters.push_back(fn);
}

void WindowsFilter::FilterWindowLeftOf(int right) {
	auto fn = [right](HWND hWnd) {
		RECT rectw;

		if (GetWindowRect(hWnd, &rectw)) {
			return rectw.right <= right;
		}
		else {
			return false;
		}
	};
	filters.push_back(fn);
}

void WindowsFilter::FilterWindowBelowOf(int top) {
	auto fn = [top](HWND hWnd) {
		RECT rectw;

		if (GetWindowRect(hWnd, &rectw)) {
			return rectw.top >= top;
		}
		else {
			return false;
		}
	};
	filters.push_back(fn);
}

void WindowsFilter::FilterWindowAboveOf(int bottom) {
	auto fn = [bottom](HWND hWnd) {
		RECT rectw;

		if (GetWindowRect(hWnd, &rectw)) {
			return rectw.bottom <= bottom;
		}
		else {
			return false;
		}
	};
	filters.push_back(fn);
}

void WindowsFilter::FilterWindowPid(int pid) {
	auto fn = [pid](HWND hWnd) {
		DWORD process_id;
		GetWindowThreadProcessId(hWnd, &process_id);
		return pid == (int)process_id;
	};
	filters.push_back(fn);
}

void WindowsFilter::FilterWindowParent(int parent) {
	auto fn = [parent](HWND hWnd) {
		return parent == (int)GetParent(hWnd);
	};
	filters.push_back(fn);
}

void WindowsFilter::FilterWindowId(long id) {
	auto fn = [id](HWND hWnd) {
		return id == GetWindowLong(hWnd, GWL_ID);
	};
	filters.push_back(fn);
}

void WindowsFilter::FilterWindowProcessName(utility::string_t text) {
	auto fn = [text](HWND hWnd) {
		DWORD process_id;
		GetWindowThreadProcessId(hWnd, &process_id);
		bool ret = true;
		
		HANDLE Handle = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, process_id);
		if (Handle) {
			TCHAR buffproc[MAX_PATH];
			DWORD psize = sizeof(buffproc);

			buffproc[0] = 0;
			if (QueryFullProcessImageName(Handle, 0, buffproc, &psize)){
				return (_tcsstr(buffproc, text.c_str()) != NULL);
			}
			CloseHandle(Handle);
		}
		return true;
	};
	filters.push_back(fn);
}

