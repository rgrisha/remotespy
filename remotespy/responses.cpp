

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
}

void respond(const http_request& request, const status_code& status, json::value& response, TCHAR* error_code) {
	json::value resp;
	resp[U("status")] = json::value::number(status);
	if (!response.is_null()) {
		resp[U("response")] = response;
	}

	if (error_code != NULL) {
		resp[U("error_code")] = json::value::string(error_code);
	}

	// Pack in the current time for debugging purposes.
	time_t now = time(0);
	utility::stringstream_t ss;
	ss << put_time(localtime(&now), L"%Y-%m-%dT%H:%S:%MZ");
	resp[U("server_time")] = json::value::string(ss.str());
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


void AnswerPutRequest(http_request& req) {

	auto http_vars = uri::split_query(req.request_uri().query());

	auto end = http_vars.end();
	auto action_name = http_vars.find(U("ac"));
	if (action_name == end) {
		respond_error(req, L"no action specified");
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

	if (found_name->second == L"dw") {
		respond(req, status_codes::OK, wf->CreateWindowsDocument()->GetWindowsDocDesktop(), NULL);
		return;
	} else if (found_name->second == L"mw") {
		MakeFiltersFromUri(http_get_vars, wf);
		respond(req, status_codes::OK, wf->CreateWindowsDocumentForMainWindows()->GetWindowsDoc(), NULL);
		return;
	} else if (found_name->second == L"cw") {
		auto hwname = http_get_vars.find(U("hwnd"));
		if (hwname == end(http_get_vars)) {
			respond(req, status_codes::BadRequest, json::value::string(L"cw query without hwnd"), NULL);
		}
		HWND hWnd = (HWND)_tstoi(hwname->second.c_str());
		MakeFiltersFromUri(http_get_vars, wf);
		respond(req, status_codes::OK, wf->CreateWindowsDocumentForChildWindows(hWnd)->GetWindowsDoc(), NULL);
	}
	respond_error(req, L"unknown query");

	//respond(req, status_codes::OK, json::value::string(U("Request received for: ") + request_name));

	delete wf;

}