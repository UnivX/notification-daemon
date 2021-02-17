#include "RenderingThread.h"
#include "FontResource.h"
#include "ResourceManager.h"
#include <memory>
#ifdef SFML_SYSTEM_LINUX
#include <unistd.h>
#endif
#include <iostream>

RenderingThread::RenderingThread()
{
	this->returnCode = 0;
	this->m_run = true;
	this->window = new OverlayWindow(sf::Vector2i(1920 - 400, 1080 - 140), sf::Vector2i(400, 100));
	this->HideWindow();
	this->runningNotification =nullptr;
}


RenderingThread::~RenderingThread()
{
	//delete all dynamic memory
	if (this->window != nullptr)
		delete this->window;
	if (this->thread != nullptr)
		delete this->thread;
	if (this->runningNotification != nullptr)
		delete this->runningNotification;
	for (int i = 0; i < this->notificationQueue.size(); i++)
		delete this->notificationQueue[i];
	this->notificationQueue.clear();
}

void RenderingThread::SetFramerate(int frameRate)
{
	std::lock_guard<std::mutex> l(this->windowMutex);
	if (this->window != nullptr)
		this->window->getWindow()->setFramerateLimit(frameRate);
}

void RenderingThread::HideWindow()
{
#ifndef SFML_SYSTEM_LINUX
	::ShowWindow(this->window->getWindow()->getSystemHandle(), SW_HIDE);
#else
	//this->window->getWindow()->setVisible(false);
#endif
}

void RenderingThread::ShowWindow()
{
#ifndef SFML_SYSTEM_LINUX
	::ShowWindow(this->window->getWindow()->getSystemHandle(), SW_SHOW);
#else
	//this->window->getWindow()->setVisible(true);
#endif
}

void RenderingThread::PlayNotification(Notification* notification)
{
	this->notificationQueueMutex.lock();
	this->notificationQueue.emplace_back(notification);
	this->notificationQueueMutex.unlock();
}

void RenderingThread::Main()
{
	/*
	sf::Text text;
	sf::Font font;
	font.loadFromFile("C:\\Users\\SIMONE\\Desktop\\Notification\\Candara.ttf");

	// select the font
	text.setFont(font); // font is a sf::Font

	// set the string to display
	text.setString("Hello world");

	// set the character size
	text.setCharacterSize(24); // in pixels, not points!

	// set the color
	text.setFillColor(sf::Color::Red);

	// set the text style
	text.setStyle(sf::Text::Bold | sf::Text::Underlined);

	text.setPosition(sf::Vector2f(0, 0));	
	*/
	sf::Clock clock;
	while (this->m_run) {
		//notification queue things

		//protected section
		this->windowMutex.lock();
		if (!this->window->getWindow()->isOpen()) {
			this->m_run = false;
			this->windowMutex.unlock();
			break;
		}
		sf::Event event;
		while (window->getWindow()->pollEvent(event))
		{
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window->getWindow()->close();
		}
		this->windowMutex.unlock();
		//end of protected section

		//update
		float dt = clock.restart().asSeconds();//get delta time

		//update notification
		this->notificationQueueMutex.lock();
		//if notification ended delete the notification and set the pointer null
		if (this->runningNotification != nullptr && this->runningNotification->IsEnded()) {
			delete this->runningNotification;
			this->runningNotification = nullptr;
			window->clear();
			window->display();
			this->HideWindow();
		}
		// if there is no running notification get one notification from the queue and set the running pointer
		if (this->notificationQueue.size() != 0 && this->runningNotification == nullptr) {
			this->ShowWindow();
			this->runningNotification = this->notificationQueue[0];
			this->window->setPosition(this->runningNotification->GetWindowPosition());
			this->window->SetSize(this->runningNotification->GetSize());
			this->notificationQueue.erase(this->notificationQueue.begin());
		}
		if(this->runningNotification != nullptr)
			this->runningNotification->Update(dt);
		this->notificationQueueMutex.unlock();

		//protected section
		this->windowMutex.lock();
		if (this->runningNotification != nullptr) {
			window->clear();

			//draw notification
			this->notificationQueueMutex.lock();
			if (this->runningNotification != nullptr)
				this->runningNotification->RenderAll(*this->window);
			//window->draw(*text->GetDrawableObject());
			this->notificationQueueMutex.unlock();
			//window->draw(text);
			window->display();
		}
		else
#ifndef SFML_SYSTEM_LINUX
			Sleep(40);
#else
			usleep(40);
#endif
		this->windowMutex.unlock();
		//end of protected section
	}
}

void RenderingThread::CloseThread()
{
	this->m_run = false;
}
