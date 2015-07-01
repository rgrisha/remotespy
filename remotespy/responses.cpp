

#include "stdafx.h"
#include <ctime>
#include <sstream>
#include <chrono>
#include <iomanip>

void MakeFiltersFromUri(uri_map& get_vars, WindowsFilter* wf) {

	auto end = get_vars.end();

	auto found_name = get_vars.find(U("wt"));
	if (found_name != end && !found_name->second.empty()) {
		wf->FilterWindowText(found_name->second);
	}
	found_name = get_vars.find(U("wc"));
	if (found_name != end && !(found_name->second.empty())) {
		wf->FilterWindowClass(found_name->second);
	}
	found_name = get_vars.find(U("left"));
	if (found_name != end && !(found_name->second.empty())) {
		int left = _tcstoul(found_name->second.c_str(), 0, 0);
		wf->FilterWindowRightOf(left);
	}
	found_name = get_vars.find(U("right"));
	if (found_name != end && !(found_name->second.empty())) {
		int right = _tcstoul(found_name->second.c_str(), 0, 0);
		wf->FilterWindowLeftOf(right);
	}
	found_name = get_vars.find(U("top"));
	if (found_name != end && !(found_name->second.empty())) {
		int top = _tcstoul(found_name->second.c_str(), 0, 0);
		wf->FilterWindowBelowOf(top);
	}
	found_name = get_vars.find(U("bottom"));
	if (found_name != end && !(found_name->second.empty())) {
		int bottom = _tcstoul(found_name->second.c_str(), 0, 0);
		wf->FilterWindowAboveOf(bottom);
	}
	found_name = get_vars.find(U("pid"));
	if (found_name != end && !(found_name->second.empty())) {
		int pid = _tcstoul(found_name->second.c_str(), 0, 0);
		wf->FilterWindowPid(pid);
	}
	found_name = get_vars.find(U("par"));
	if (found_name != end && !(found_name->second.empty())) {
		int parent = _tcstoul(found_name->second.c_str(), 0, 0);
		wf->FilterWindowParent(parent);
	}
	found_name = get_vars.find(U("id"));
	if (found_name != end && !(found_name->second.empty())) {
		int id = _tcstoul(found_name->second.c_str(), 0, 0);
		wf->FilterWindowId(id);
	}
	found_name = get_vars.find(U("process"));
	if (found_name != end && !(found_name->second.empty())) {
		wf->FilterWindowProcessName(found_name->second);
	}

}

void respond(const http_request& request, const status_code& status, json::value& response, TCHAR* error_code) {
	json::value resp;
	resp[U("status")] = json::value::number(status);
	if (!response.is_null()) {
		resp[U("result")] = response;
	}

	if (error_code != NULL) {
		resp[U("errorCode")] = json::value::string(error_code);
	}

	// Pack in the current time for debugging purposes.
	utility::stringstream_t ss;

	struct tm tm;
	auto t = std::time(nullptr);
	localtime_s(&tm, &t);
	ss << std::put_time(&tm, _T("%Y-%m-%dT%H-%M-%SZ"));
	resp[U("serverTime")] = json::value::string(ss.str());
	
	request.reply(status, resp);
}

void respond_ok(const http_request& request ) {
	json::value jnull = json::value::Null;
	respond(request, status_codes::OK, jnull, NULL);
}

void respond_error(const http_request& request, TCHAR* error_code) {
	json::value jnull = json::value::Null;
	respond(request, status_codes::InternalError, jnull, error_code);
}

HWND get_hwnd(uri_map& vars) {
	auto found_name = vars.find(U("w"));
	if (found_name == vars.end()) {
		return NULL;
	} else {
		HWND hWnd = (HWND)_tstoi(found_name->second.c_str());
		return hWnd;
	}
}

BOOL get_xy(uri_map& vars, int* x, int* y) {
	auto found_name_x = vars.find(U("x"));
	if (found_name_x == vars.end()) {
		return FALSE;
	} else {
		auto found_name_y = vars.find(U("y"));
		if (found_name_y == vars.end()) {
			return FALSE;
		} else {
			*x = _tstoi(found_name_x->second.c_str());
			*y = _tstoi(found_name_y->second.c_str());
			return TRUE;
		}
	}
}

void answer_settext(http_request req, uri_map& vars) {
	auto end = vars.end();
	if (HWND hWnd = get_hwnd(vars)) {
		auto txt = vars.find(U("txt"));
		if (txt == end) {
			respond_error(req, L"no text");
			return;
		}
		const TCHAR* tt = txt->second.c_str();
		if (SetText(hWnd, tt) == 0) {
			respond_ok(req);
		} else {
			respond_error(req, L"set text error");
		}
	} else {
		respond_error(req, L"no hwnd");
	}
}

void answer_click(http_request req, uri_map& vars) {
	auto end = vars.end();
	if (HWND hWnd = get_hwnd(vars)) {
		if (ClickMouse(hWnd) == 0) {
			respond_ok(req);
		} else {
			respond_error(req, L"click error");
		}
	} else {
		respond_error(req, L"no hwnd");
	}
}

void answer_sendinput(http_request req, uri_map& vars) {
	auto end = vars.end();
	auto txt = vars.find(U("txt"));
	if (txt == end) {
		respond_error(req, L"no text");
		return;
	}
	const TCHAR* text = txt->second.c_str();
	SendKeyInput(text);
	respond_ok(req);
}

void answer_mousemove(http_request req, uri_map& vars) {
	auto end = vars.end();
	if (HWND hWnd = get_hwnd(vars)) {
		MoveMouse(hWnd);
		respond_ok(req);
		return;
	}
	int x; int y;
	if (get_xy(vars, &x, &y)) {
		MoveMouse(x, y);
		respond_ok(req);
		return;
	}
	respond_error(req, L"neither hwnd nor x,y specified");
}

void answer_window_close(http_request req, uri_map& vars) {
	auto end = vars.end();
	if (HWND hWnd = get_hwnd(vars)) {
		CloseWindow(hWnd);
		respond_ok(req);
		return;
	}
}


void AnswerPutRequest(http_request& req) {

	auto http_vars = uri::split_query(req.request_uri().query());

	auto end = http_vars.end();
	auto action_name = http_vars.find(_T("ac"));
	if (action_name == end) {
		respond_error(req, _T("no action specified"));
		return;
	}

	if (action_name->second == L"click") {
		answer_click(req, http_vars);
	} else if (action_name->second == L"mousemove") {
		answer_mousemove(req, http_vars);
	} else if (action_name->second == L"wtext") {
		answer_settext(req, http_vars);
	} else if (action_name->second == L"sendtext") {
		answer_sendinput(req, http_vars);
	}
	else if (action_name->second == L"close") {
		answer_window_close(req, http_vars);
	} else {
		respond_error(req, L"bad action");
	}
}

void AnswerGetRequest(http_request& req) {

	auto http_get_vars = uri::split_query(req.request_uri().query());
	auto found_name = http_get_vars.find(U("q"));

	if (found_name == end(http_get_vars)) {
		auto err = U("Request received with get var \"q\" omitted from query.");
		uclog << err << endl;
		respond_error(req, L"no q in query");
		return;
	}

	auto request_name = found_name->second;

	uclog << U("Received request: ") << request_name << endl;

	WindowsFilter* wf = new WindowsFilter();
	json::value windows;

	if (found_name->second == L"dw") {
		windows[_T("windows")] = wf->CreateWindowsDocument()->GetWindowsDocDesktop();
		respond(req, status_codes::OK, windows , NULL);
	} else if (found_name->second == L"mw") {
		MakeFiltersFromUri(http_get_vars, wf);
		windows[_T("windows")] = wf->CreateWindowsDocumentForMainWindows()->GetWindowsDoc();
		respond(req, status_codes::OK, windows, NULL);		
	} else if (found_name->second == L"cw") {
		auto hwname = http_get_vars.find(U("hwnd"));
		if (hwname == end(http_get_vars)) {
			respond_error(req, L"cw query without hwnd");
		}

		HWND hWnd = (HWND)_tcstoul(hwname->second.c_str(), 0, 0);
		MakeFiltersFromUri(http_get_vars, wf);
		windows[_T("windows")] = wf->CreateWindowsDocumentForChildWindows(hWnd)->GetWindowsDoc();

		respond(req, status_codes::OK, windows, NULL);		
	} else {
		respond_error(req, L"unknown query");
	}

	delete wf;
}