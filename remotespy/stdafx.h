// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <psapi.h>
#include <cpprest/json.h>
#include <cpprest/http_msg.h>
#include <cpprest/http_listener.h>


// TODO: reference additional headers your program requires here
#include "utils.h"
#include "windoc.h"
#include "winfilter.h"
#include "responses.h"
#include "actions.h"
#include "uri_map.h"


#ifdef _UTF16_STRINGS
	// On Windows, all strings are wide
#define uclog std::wclog
#else
	// On POSIX platforms, all strings are narrow
#define uclog std::clog
#endif // endif _UTF16_STRINGS