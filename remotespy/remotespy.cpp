// remotespy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>
#include <thread>

//#pragma comment(lib, "cpprest110_1_1")

using namespace std;
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

#define _CRT_SECURE_NO_DEPRECATE
	// cpprest provides macros for all streams but std::clog in basic_types.h


void print_usage(TCHAR* exe) {
	_tprintf(_T("\nUsage: %s -host <http://hostname:port>\n\n"), exe);
	_tprintf(_T("To register listener on Windows, use: netsh http add urlacl url=<url> user=<user>\n"));
	_tprintf(_T("Example: netsh http add urlacl url=http://+:50008/ user=Everyone\n\n"));
	_tprintf(_T("Do not forget to open port in Windows Firewall if using remote connection\n"));
}

int _tmain(int argc, _TCHAR* argv[]) {

	TCHAR* hostname = NULL;

	// a simple parameter parser
	for (int i = 0; i < argc; i++) {
		if (_tcscmp(argv[i], _T("-host")) == 0 && argc > i + 1) {
			hostname = argv[i + 1];
		}
	}

	if (hostname == NULL) {
		print_usage(argv[0]);
		return 1;
	}

	// Synchronously bind the listener to all nics.
	uclog << _T("Starting listener on hostname ") << hostname << endl;
	http_listener listener(hostname);

	listener.open().wait();

	// Handle incoming requests.
	uclog << U("Setting up JSON listener.") << endl;

	listener.support(methods::PUT, [](http_request req) {
		AnswerPutRequest(req);
	});

	listener.support(methods::GET, [](http_request req) {
		AnswerGetRequest(req);
	});


	// Wait while the listener does the heavy lifting.
	// TODO: Provide a way to safely terminate this loop.
	uclog << U("Waiting for incoming connection...") << endl;

	while (true) {
		this_thread::sleep_for(chrono::milliseconds(2000));
	}

	// Nothing left to do but commit suicide.
	uclog << U("Terminating JSON listener.") << endl;
	listener.close();

	return 0;
}

