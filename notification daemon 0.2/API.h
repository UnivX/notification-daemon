#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <SFML/Graphics.hpp>
#include <iostream>
#include "SocketThread.h"
#include <memory>
#include "ResourceManager.h"
#include "RenderingThread.h"
#include "utilities.h"
#include "NotificatonBluePrintResource.h"
#include "TextureResource.h"
#include "FontResource.h"
#include "utilities.h"
#include "Animations.h"
#include <sstream>

class Api {
public:
	Api(RenderingThread& renderingThread);
	void ElaborateMessage(std::string msg, SocketThread& thread);
private:
	std::string StartNotificationResourceCreation(std::string msg);
	std::string EndNotificationResourceCreation(std::string msg);

	std::string LoadTextureResource(std::string msg);
	std::string LoadFontResource(std::string msg);

	std::string AddRectangleToNotificationResource(std::string msg);
	std::string AddCircleToNotificationResource(std::string msg);
	std::string AddSpriteToNotificationResource(std::string msg);
	std::string AddTextToNotificationResource(std::string msg);
	std::string AddTextBoxToNotificationResource(std::string msg);// TODO

	std::string PopUpNotification(std::string msg);
	std::string GetLoadedResources(std::string msg);

	std::string	CloseDaemon();


	std::string tempNotififcatonBluePrintName;
	Notification* tempNotificationPtr;
	RenderingThread* renderingThread;
	ResourceManager* resourceManagerInstance;
};




struct RawMessages {
	bool errorsOnReciving;
	std::vector<std::string> messages;
};

//use basic json data
RawMessages GetMessages(SocketThread& thread);//chek needed

template<class T>
bool GetValueFromMessage(std::string message, std::string variable, T& value);

template<class T>
inline bool GetValueFromMessage(std::string message, std::string variable, T& value)
{
	try
	{
		variable = "\"" + variable + "\"";
		auto position = message.find(variable.c_str());
		auto firstPosition = message.find('"', position + variable.size() + 1);
		auto secondPosition = message.find('"', firstPosition + 1);

		if (position == std::string::npos || firstPosition == std::string::npos || secondPosition == std::string::npos)
			return false;
		std::stringstream ss;
		ss << message.substr(firstPosition + 1, secondPosition - firstPosition - 1);
		ss >> value;
		return true;
	}
	catch (const std::exception & exc)
	{
		#ifndef SFML_SYSTEM_LINUX
		MessageBoxA(NULL, "Notification Daemon, GetValueFromMessage failed", exc.what(), MB_ICONERROR);
		return false;
		#else
		return false;
		#endif
	}
	return false;
}


inline bool GetValueFromMessage(std::string message, std::string variable, std::string& value){
	try
	{
		variable = "\"" + variable + "\"";
		auto position = message.find(variable.c_str());
		auto firstPosition = message.find('"', position + variable.size() + 1);
		auto secondPosition = message.find('"', firstPosition + 1);

		auto tempPos = firstPosition;
		while (message[secondPosition - 1] == '\\') {
			tempPos++;
			secondPosition = message.find('"', tempPos + 1);
		}

		if (position == std::string::npos || firstPosition == std::string::npos || secondPosition == std::string::npos)
			return false;
		std::stringstream ss;
		value = message.substr(firstPosition + 1, secondPosition - firstPosition - 1);
		replaceAll(value, "\\\\n", "\\n");//do the \n method
		replaceAll(value, "\\n", "\n");//do the \n method
		replaceAll(value, "\\\"", "\"");//do the \" method
		return true;
	}
	catch (const std::exception & exc)
	{
		#ifndef SFML_SYSTEM_LINUX
		MessageBoxA(NULL, "Notification Daemon, GetValueFromMessage<std::string> failed", exc.what(), MB_ICONERROR);
		return false;
		#else
		return false;
		#endif
	}
	return false;
}
