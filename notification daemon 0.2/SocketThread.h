#pragma once
#include <SFML/Graphics.hpp>

#ifndef SFML_SYSTEM_LINUX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#else

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>

#endif

#include <atomic>
#include <functional>
#include <future>
//error codes
#define SOCKET_THREAD_NO_ERROR 0
#define SOCKET_THREAD_ERROR_GET_ADDRESS_FAILED 1
#define SOCKET_THREAD_ERROR_SOCKET_FAILED 2
#define SOCKET_THREAD_ERROR_BIND_FAILED 3
#define SOCKET_THREAD_ERROR_LISTEN_FAILED 4
#define SOCKET_THREAD_ERROR_ACCEPT_FAILED 5
#define SOCKET_THREAD_ERROR_SEND_FAILED 6
#define SOCKET_THREAD_ERROR_RECV_FAILED 7
//connection status codes
#define SOCKET_STATUS_NOT_CONNECTED 0
#define SOCKET_STATUS_LISTENING 1
#define SOCKET_STATUS_CONNECTED 2
#define SOCKET_STATUS_CLOSED 3

//need to be thread safe
/*
this is a class for multi thread socket listening with high level of error catch
the functions send and recive are created to be used only by the socket thread, they are't thread safe
the thread will execute the intern function only if the server socket is connected
all the other functions are thread safe
the class is not copiable
*/
class SocketThread
{
public:
#ifndef SFML_SYSTEM_LINUX
	SocketThread(PCSTR port, int bufferLenght, std::function<void(std::atomic<int>& returnCode, std::atomic<int>& connectionStatus, SocketThread& thread)> internFunction);
#else
	SocketThread(int port, int bufferLenght, std::function<void(std::atomic<int>& returnCode, std::atomic<int>& connectionStatus, SocketThread& thread)> internFunction);
#endif
	~SocketThread();
	int GetReturnCode();
	int GetSocketConnectionStatus();
	int GetBufferLenght();
	bool Send(const char* buffer);
	bool Recv(char* buffer);
	void StartThread();
	bool IsThreadRunning();
private:
	void MainLoop();
	void Connect();
	void Debug(int n);
#ifndef SFML_SYSTEM_LINUX
	PCSTR port;
#endif
	std::future<void> m_future;
	std::atomic<int> returnCode, connectionStatus;
	std::function<void(std::atomic<int>& returnCode, std::atomic<int>& connectionStatus, SocketThread& thread)> internFunction;
#ifndef SFML_SYSTEM_LINUX
	SOCKET listeningSocket, clientSocket;
#else
	int sockfd, newsockfd, port;
#endif
	int bufferLenght;
};

