#include "SocketThread.h"
#include <iostream>
#ifdef SFML_SYSTEM_LINUX
#include <X11/Xlib.h>
#endif

#ifndef SFML_SYSTEM_LINUX
SocketThread::SocketThread(PCSTR port, int bufferLenght, std::function<void(std::atomic<int>& returnCode, std::atomic<int>& connectionStatus, SocketThread& thread)> internFunction)
{
	this->port = port;
	this->bufferLenght = bufferLenght;
	this->internFunction = internFunction;

}
#else

SocketThread::SocketThread(int port, int bufferLenght, std::function<void(std::atomic<int>& returnCode, std::atomic<int>& connectionStatus, SocketThread& thread)> internFunction)
{
	this->port = port;
	this->bufferLenght = bufferLenght;
	this->internFunction = internFunction;
	this->returnCode = SOCKET_THREAD_NO_ERROR;
	this->connectionStatus = SOCKET_STATUS_NOT_CONNECTED;
}

#endif

SocketThread::~SocketThread()
{
}

int SocketThread::GetReturnCode()
{
	return this->returnCode;
}

int SocketThread::GetSocketConnectionStatus()
{
	return this->connectionStatus;
}

int SocketThread::GetBufferLenght()
{
	return this->bufferLenght;
}
#ifndef SFML_SYSTEM_LINUX
bool SocketThread::Send(const char* buffer)
{
	int iSendResult = send(this->clientSocket, buffer, this->bufferLenght, 0);
	if (iSendResult == SOCKET_ERROR) {
		this->returnCode = SOCKET_THREAD_ERROR_SEND_FAILED;
		closesocket(this->clientSocket);
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		return false;
	}
	return true;
}
#else
bool SocketThread::Send(const char* buffer)
{
	std::cout << "sending: " << buffer << std::endl;
	if(this->newsockfd == 0){
		std::cout << "error: socket closed" << std::endl; 
		this->returnCode = SOCKET_THREAD_ERROR_SEND_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		return false;
	}
	if(send(this->newsockfd, buffer, this->bufferLenght, 0) == -1){
		perror("error: on send");
		this->returnCode = SOCKET_THREAD_ERROR_SEND_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		return false;
	}
	return true;
}
#endif

#ifndef SFML_SYSTEM_LINUX
bool SocketThread::Recv(char* buffer)
{
	int iResult = recv(this->clientSocket, buffer, this->bufferLenght, 0);
	if (!(iResult > 0)) {
		this->returnCode = SOCKET_THREAD_ERROR_RECV_FAILED;
		closesocket(this->clientSocket);
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		return false;
	}

	return true;
}
#else
bool SocketThread::Recv(char* buffer)
{
	if(this->newsockfd == 0){
		std::cout << "error: socket closed" << std::endl; 
		this->returnCode = SOCKET_THREAD_ERROR_RECV_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		return false;
	}
	int iResult = recv(newsockfd,buffer,this->bufferLenght, 0);
	if (iResult <= 0) {
		perror("error: on recv");
		this->returnCode = SOCKET_THREAD_ERROR_RECV_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		return false;
	}

	return true;
}
#endif

void SocketThread::StartThread()
{
	this->m_future = std::async(std::launch::async, &SocketThread::MainLoop, this);
	this->connectionStatus = SOCKET_STATUS_LISTENING;
}

bool SocketThread::IsThreadRunning()
{
	if(this->m_future.valid())
		return this->m_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready;//return if the return is avaible(if the return of the function is avaible the thread has stopped
	return false;
}

#ifndef SFML_SYSTEM_LINUX
void SocketThread::MainLoop()
{
	this->returnCode = SOCKET_THREAD_NO_ERROR;
	//try to connect
	this->Connect();
	if (this->returnCode != SOCKET_THREAD_NO_ERROR || this->connectionStatus != SOCKET_STATUS_CONNECTED)//if not connected or some error occured terminate thread
		return;

	this->internFunction(this->returnCode, this->connectionStatus, *this);

	//close the socket
	closesocket(this->listeningSocket);
	// shutdown the connection since we're done
	if (shutdown(this->clientSocket, SD_SEND) == SOCKET_ERROR) {
		closesocket(this->clientSocket);
		return;
	}

	// cleanup
	this->connectionStatus = SOCKET_STATUS_CLOSED;
	closesocket(this->clientSocket);
}
#else
	void SocketThread::MainLoop()
{
	XInitThreads();
	this->returnCode = SOCKET_THREAD_NO_ERROR;
	//try to connect
	this->Connect();
	if (this->returnCode != SOCKET_THREAD_NO_ERROR || this->connectionStatus != SOCKET_STATUS_CONNECTED)//if not connected or some error occured terminate thread
		return;
	this->Debug(1);
	this->internFunction(this->returnCode, this->connectionStatus, *this);
	// cleanup
	this->Debug(2);
	this->connectionStatus = SOCKET_STATUS_CLOSED;
	this->newsockfd = 0;
}
#endif

#ifndef SFML_SYSTEM_LINUX
void SocketThread::Connect()
{
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	int iResult = 0;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, this->port, &hints, &result);
	if (iResult != 0) {
		this->returnCode = SOCKET_THREAD_ERROR_GET_ADDRESS_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		return;
	}

	// Create a SOCKET for connecting to server
	this->listeningSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listeningSocket == INVALID_SOCKET) {
		this->returnCode = SOCKET_THREAD_ERROR_SOCKET_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		freeaddrinfo(result);
		return;
	}

	// Setup the TCP listening socket
	iResult = bind(this->listeningSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		this->returnCode = SOCKET_THREAD_ERROR_BIND_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		freeaddrinfo(result);
		closesocket(this->listeningSocket);
		return;
	}

	freeaddrinfo(result);
	iResult = listen(this->listeningSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		this->returnCode = SOCKET_THREAD_ERROR_LISTEN_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		closesocket(this->listeningSocket);
		return;
	}

	// Accept a client socket
	this->clientSocket = accept(this->listeningSocket, NULL, NULL);
	if (this->clientSocket == INVALID_SOCKET) {
		this->returnCode = SOCKET_THREAD_ERROR_ACCEPT_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		closesocket(this->listeningSocket);
		return;
	}

	// No longer need server socket
	this->connectionStatus = SOCKET_STATUS_CONNECTED;
	closesocket(this->listeningSocket);
}
#else
void SocketThread::Connect()
{
	struct sockaddr_in serv_addr, cli_addr;
    int n;
	socklen_t clilen;
    // create a socket
    // socket(int domain, int type, int protocol)
    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
		perror("failed creating socket");
		close(sockfd);
    	this->returnCode = SOCKET_THREAD_ERROR_SOCKET_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		return;
	}

	int tr=1;

	// kill "Address already in use" error message
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

    // clear address structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(this->port);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		this->returnCode = SOCKET_THREAD_ERROR_BIND_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		close(sockfd);
		this->sockfd = 0;
		perror("bind");
		return;
	}
	

	if(listen(sockfd,5) < 0){
		this->returnCode = SOCKET_THREAD_ERROR_LISTEN_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		close(sockfd);
		this->sockfd = 0;
		perror("listen");
		return;
	}


    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
		this->returnCode = SOCKET_THREAD_ERROR_ACCEPT_FAILED;
		this->connectionStatus = SOCKET_STATUS_CLOSED;
		close(sockfd);
		close(newsockfd);
		perror("accept");
		return;
	}
	close(sockfd);
	this->connectionStatus = SOCKET_STATUS_CONNECTED;
	
}
#endif

void SocketThread::Debug(int n){ return; }