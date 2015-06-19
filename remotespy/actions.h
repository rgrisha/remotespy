
#ifndef _ACTIONS_HEADER_
#define _ACTIONS_HEADER_

#include "stdafx.h"

int MoveMouse(int x, int y);
int MoveMouse(HWND hWnd);
int ClickMouse(HWND hWnd);
int SetText(HWND hWnd, const TCHAR* msg);
BOOL SendKeyInput(const TCHAR* txt);

#endif //#ifndef _ACTIONS_HEADER_