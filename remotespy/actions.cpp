#include "stdafx.h"

int MoveMouse(int x, int y) {

	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwExtraInfo = 0;
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	input.mi.mouseData = 0;
	input.mi.time = 0;
	input.mi.dx = (65535 / GetSystemMetrics(SM_CXSCREEN)) * x;
	input.mi.dy = (65535 / GetSystemMetrics(SM_CYSCREEN)) * y;
	SendInput(1, &input, sizeof(input));

	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &input, sizeof(input));

	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &input, sizeof(input));

	return 0;

}

int MoveMouse(HWND hWnd) {

	RECT rect;
	if (!GetWindowRect(hWnd, &rect)) {
		uclog << U("Could not get window rect for HWND=") << (int)hWnd;
		return -1;
	}

	rect.left++;
	rect.top++;

	MoveMouse(rect.left, rect.top);

	return 0;
}

int ClickMouse(HWND hWnd) {

	RECT rect;
	if (!GetWindowRect(hWnd, &rect)) {
		uclog << U("Could not get window rect for HWND=") << (int)hWnd;
		return -1;
	}

	int x = (rect.right - rect.left) >> 1;
	int y = (rect.bottom - rect.top) >> 1;

	SetForegroundWindow(hWnd);
	SetActiveWindow(hWnd);
	SetFocus(hWnd);
	Sleep(300);
	SetCursorPos(x, y);

	return 0;
}

int SetText(HWND hWnd, const TCHAR* msg) {
	if (::SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)msg)) {
		return 0;
	} else {
		return -1;
	}
}

/*
int CloseWindow(HWND hWnd) {
	if (::SendMessage(hWnd, WM_CLOSE, 0, 0)) {
		return 0;
	}
	else {
		return -1;
	}
}
*/

//stolen from
//https://nibuthomas.wordpress.com/2009/08/04/how-to-use-sendinput/
BOOL SendKeyInput(const TCHAR* lpctszText) {
	vector<INPUT> EventQueue;

	TCHAR Buff[120] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILANGUAGE, Buff, sizeof(Buff));
	HKL hKeyboardLayout = ::LoadKeyboardLayout(Buff, KLF_ACTIVATE);

	const int Len =  _tcslen(lpctszText);
	for (int Index = 0; Index < Len; ++Index) {
		INPUT Event = { 0 };

		const SHORT Vk = VkKeyScanEx(lpctszText[Index], hKeyboardLayout);
		const UINT VKey = ::MapVirtualKey(LOBYTE(Vk), 0);

		if (HIBYTE(Vk) == 1) { // Check if shift key needs to be pressed for this key
			// Press shift key
			::ZeroMemory(&Event, sizeof(Event));
			Event.type = INPUT_KEYBOARD;
			Event.ki.dwFlags = KEYEVENTF_SCANCODE;
			Event.ki.wScan = ::MapVirtualKey(VK_LSHIFT, 0);
			EventQueue.push_back(Event);
		}

		// Keydown
		::ZeroMemory(&Event, sizeof(Event));
		Event.type = INPUT_KEYBOARD;
		Event.ki.dwFlags = KEYEVENTF_SCANCODE;
		Event.ki.wScan = VKey;
		EventQueue.push_back(Event);

		// Keyup
		::ZeroMemory(&Event, sizeof(Event));
		Event.type = INPUT_KEYBOARD;
		Event.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		Event.ki.wScan = VKey;
		EventQueue.push_back(Event);

		if (HIBYTE(Vk) == 1)// Release if previouly pressed
		{
			// Release shift key
			::ZeroMemory(&Event, sizeof(Event));
			Event.type = INPUT_KEYBOARD;
			Event.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
			Event.ki.wScan = ::MapVirtualKey(VK_LSHIFT, 0);
			EventQueue.push_back(Event);
		}
	}// End for

	if (hKeyboardLayout) {
		UnloadKeyboardLayout(hKeyboardLayout);
	}

	return ::SendInput(EventQueue.size(), &EventQueue[0], sizeof(INPUT));
}