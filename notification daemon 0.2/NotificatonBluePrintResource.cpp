#include "NotificatonBluePrintResource.h"



NotificationBluePrintResource::NotificationBluePrintResource(std::string name)
{
	this->SetName(name);
	this->notificationBluePrint = nullptr;
}


void NotificationBluePrintResource::LoadFromNotification(Notification* notification)
{
	this->notificationBluePrint = notification;
}

NotificationBluePrintResource::~NotificationBluePrintResource()
{
	delete this->notificationBluePrint;
}

Notification* NotificationBluePrintResource::GetNotificationCopy()
{
	if(this->notificationBluePrint == nullptr)
		return nullptr;
	return this->notificationBluePrint->CreateDynamicCopy();
}
