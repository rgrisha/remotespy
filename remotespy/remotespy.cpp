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



int _tmain(int argc, _TCHAR* argv[]) {

// Synchronously bind the listener to all nics.
	uclog << U("Starting listener.") << endl;
	http_listener listener(U("http://+:50008/json"));

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

