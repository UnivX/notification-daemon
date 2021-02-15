#include "SocketWatchDog.h"
#include <sstream>
SocketWatchDog* SocketWatchDog::instance = nullptr;

SocketWatchDog::SocketWatchDog()
{
	this->m_run = true;
	this->m_thread = new std::thread(&SocketWatchDog::Main, this);
	this->listeningSocket = nullptr;
}


SocketWatchDog::~SocketWatchDog()
{
	if (this->m_thread != nullptr)
		delete this->m_thread;
}

SocketWatchDog* SocketWatchDog::GetInstance()
{
	if (instance == nullptr) {
		instance = new SocketWatchDog();
	}
	return instance;
}

void SocketWatchDog::RegisterNewSocket(std::shared_ptr<SocketThread> newSocket)
{
	try
	{
		//misterius bug here
		this->socketPoolMutex.lock();
		this->socketPool.emplace_back(newSocket);
		this->listeningSocket = newSocket;
		this->socketPoolMutex.unlock();
	}
	catch (const std::exception& exc)
	{
		#ifndef SFML_SYSTEM_LINUX
		MessageBoxA(NULL, "Notification Daemon, SocketWatchDog::RegisterNewSocket failed", exc.what(), MB_ICONERROR);
		#else
		;
		#endif
	}
}

void SocketWatchDog::Close()
{
	this->m_run = false;
}

void SocketWatchDog::SetCreateNewListeningSocketFunction(std::function<void(void)> CreateNewListeningSocket)
{
	this->CreateNewListeningSocket = CreateNewListeningSocket;
}

void SocketWatchDog::Main()
{
	while (this->m_run = true) {
		//if the listening thread is dead

		this->socketPoolMutex.lock();
		if (this->listeningSocket != nullptr && (this->listeningSocket->GetSocketConnectionStatus() != SOCKET_STATUS_LISTENING || !this->listeningSocket->IsThreadRunning()))//if the listening thread is dead
		{
			this->socketPoolMutex.unlock();
			this->CreateNewListeningSocket();
			this->socketPoolMutex.lock();
		}
		try
		{
			for (int i = 0; i < this->socketPool.size(); i++) {

				//if the socket isn't running
				if (!this->socketPool[i]->IsThreadRunning()) {

					if (this->socketPool[i]->GetReturnCode() != SOCKET_THREAD_NO_ERROR
						&& this->socketPool[i]->GetReturnCode() != SOCKET_THREAD_ERROR_RECV_FAILED
						&& this->socketPool[i]->GetReturnCode() != SOCKET_THREAD_ERROR_SEND_FAILED)//if some error appened
						MessageBoxA(NULL, ("a socket thread crashed" + std::to_string(WSAGetLastError())).c_str(), "ERROR", NULL);
					this->socketPool.erase(this->socketPool.begin() + i);//erase from list
					i--;
				}
			}
		}
		catch (const std::exception & exc)
		{
			#ifndef SFML_SYSTEM_LINUX
			MessageBoxA(NULL, "Notification Daemon, SocketWatchDog::Main failed", exc.what(), MB_ICONERROR);
			#else
			;
			#endif
		}

		this->socketPoolMutex.unlock();
#ifndef SFML_SYSTEM_LINUX
		Sleep(50);
#else
		sleep(50);
#endif
	}
}
