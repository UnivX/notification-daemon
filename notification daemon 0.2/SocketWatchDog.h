#pragma once
#include "SocketThread.h"
#include <thread>
#include <vector>
#include <memory>
/*
this is the watch dog to check if the socket engine is working without problems
if for some reason the listening thread crashes the watch dog will start another one with the function of recovery
every new socket must be registered
this class is based on the singleton programming pattern
*/
class SocketWatchDog
{
public:
	SocketWatchDog();
	~SocketWatchDog();
	static SocketWatchDog* GetInstance();
	void RegisterNewSocket(std::shared_ptr<SocketThread> newSocket);
	void Close();
	void SetCreateNewListeningSocketFunction(std::function<void(void)> CreateNewListeningSocket);
private:
	void Main();
	std::function<void(void)> CreateNewListeningSocket;
	static SocketWatchDog* instance;
	std::mutex socketPoolMutex;
	std::vector<std::shared_ptr<SocketThread>> socketPool;
	std::shared_ptr<SocketThread> listeningSocket;
	std::thread *m_thread;
	std::atomic<bool> m_run;
};

