#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include "Notification.h"
#include <atomic>
/*
this is the class that own the font resource
*/
class NotificationBluePrintResource : public Resource
{
public:
	void LoadFromNotification(Notification* notification);
	NotificationBluePrintResource(std::string name);
	~NotificationBluePrintResource();
	Notification* GetNotificationCopy();
private:
	Notification* notificationBluePrint;
	std::atomic<bool> loaded;//is atomic so i dont need mutex to be thread safe
};