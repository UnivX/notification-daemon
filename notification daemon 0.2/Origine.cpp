#pragma once
#define BLEN 512
#define PORT "27015"

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <windows.h>
#include "OverlayWindow.h"
#include <iostream>
#include "SocketWatchDog.h"
#include <future>
#include <algorithm>
#include <memory>
#include "API.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
/*
TODO
-implent the variables on the notification class for more reusable notification blueprint
-BIG FEATURE: make it for linux
*/
void Init() {
	//init the resource manager
	auto resourceManagerInstance = ResourceManager::GetInstance();
	resourceManagerInstance->AddNewList("Fonts");//set up fonts list
	resourceManagerInstance->AddNewList("Textures");// set up texture list
	resourceManagerInstance->AddNewList("NotificationBluePrints");// set up texture list


	// Initialize Winsock
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	Init();
	//set up rendering thread
	RenderingThread renderingThread;
	renderingThread.SetFramerate(75);

	//declaring the function of the socket thread
	std::function<void(std::atomic<int> & returnCode, std::atomic<int> & connectionStatus, SocketThread & thread)> socketFunction;

	socketFunction = [&](std::atomic<int> & returnCode, std::atomic<int> & connectionStatus, SocketThread & thread) {
		//create new listening socket
		std::cout << "new socket connected\n";
		std::shared_ptr<SocketThread> st = std::make_shared<SocketThread>(PORT, BLEN, socketFunction);
		st->StartThread();
		SocketWatchDog::GetInstance()->RegisterNewSocket(st);
		

		Api api(renderingThread);

		while (thread.IsThreadRunning()) {
			RawMessages messages = GetMessages(thread);
			if (messages.errorsOnReciving){
				returnCode = -1;
				return;
			}

			//elaborate all the messages
			for(std::string msg : messages.messages)
				api.ElaborateMessage(msg, thread);
			
			Sleep(1);
		}
		std::cout << "socket closed\n";
	};

	//initializing SocketWatchDog
	//set the function of recovery in case the listening socket die
	SocketWatchDog::GetInstance()->SetCreateNewListeningSocketFunction([&]() {
		std::shared_ptr<SocketThread> st = std::make_shared<SocketThread>(PORT, BLEN, socketFunction);
		st->StartThread();
		SocketWatchDog::GetInstance()->RegisterNewSocket(st);
		});

	//start first listening socket
	std::shared_ptr<SocketThread> st = std::make_shared<SocketThread>(PORT, BLEN, socketFunction);
	st->StartThread();
	SocketWatchDog::GetInstance()->RegisterNewSocket(st);

	renderingThread.Main();
	WSACleanup();
	return 0;
}