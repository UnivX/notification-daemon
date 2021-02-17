#pragma once
#include <SFML/Graphics.hpp>

#ifndef SFML_SYSTEM_LINUX
#include <Windows.h>
#else

#endif

#include <atomic>
#include <functional>
#include <future>
#include <thread>
#include "OverlayWindow.h"
#include "Notification.h"
/*
this is the thread that id going to own the window and draw and will own the Notification instace
the thread start when is called the constructor
its possibile to chek the return code of the thread
*/
class RenderingThread
{
public:
	RenderingThread();
	~RenderingThread();
	void SetFramerate(int frameRate);
	void HideWindow();
	void ShowWindow();
	void PlayNotification(Notification* notification);
	void Main();
	void CloseThread();
private:

	OverlayWindow* window;
	std::mutex windowMutex, notificationQueueMutex;
	std::vector<Notification*> notificationQueue;
	Notification* runningNotification;
	std::atomic<int> returnCode;
	std::atomic<bool> m_run;
	std::thread* thread;
};

