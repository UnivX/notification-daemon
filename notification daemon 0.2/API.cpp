#include "API.h"
//example of message
//{"name of object":"data", "name of object":"data"}
//onject inside object are not supported
RawMessages GetMessages(SocketThread& thread)
{
	RawMessages rawMessages;
	rawMessages.errorsOnReciving = false;
	int BLEN = thread.GetBufferLenght();
	int openedMessages = 0;

	char* recived = (char*)malloc(BLEN);
	if (recived == nullptr) {
		rawMessages.errorsOnReciving = true;
		return rawMessages;
	}
	ZeroMemory(recived, BLEN);
	try
	{
		do
		{
			bool recvReturn = thread.Recv(recived);
			if (recvReturn) {
				std::string data(recived);
				auto openGraphsLocations = findLocation(data, '{');
				auto closeGraphsLocations = findLocation(data, '}');
				openedMessages += openGraphsLocations.size() - closeGraphsLocations.size();
				if (closeGraphsLocations.size() > 0)
					for (int i = 0; i < closeGraphsLocations.size(); i++)
						rawMessages.messages.emplace_back(data.substr(openGraphsLocations[i], closeGraphsLocations[i] - openGraphsLocations[i]));
				if (openGraphsLocations.size() - closeGraphsLocations.size())
					rawMessages.messages.emplace_back("{\"code\":\"error\", \"error\":\"invalid message sended\"}");
			}
		} while (openedMessages > 0);
	}
	catch (const std::exception & exc)
	{
		MessageBoxA(NULL, "Notification Daemon, GetMessages failed", exc.what(), MB_ICONERROR);
	}
	free(recived);
	return rawMessages;
}

Api::Api(RenderingThread& renderingThread)
{
	this->renderingThread = &renderingThread;
	this->resourceManagerInstance = ResourceManager::GetInstance();
	this->tempNotificationPtr = nullptr;
	this->tempNotififcatonBluePrintName = "";
}

void Api::ElaborateMessage(std::string msg, SocketThread& thread)
{
	char* sendBuffer = (char*)malloc(thread.GetBufferLenght());
	if (sendBuffer == nullptr) {
		return;
	}
	ZeroMemory(sendBuffer, thread.GetBufferLenght());

	std::string response = "";
	std::string code;

	if (!GetValueFromMessage(msg, "code", code))
		return;

	if (code == "start_notification_resource_creation")
		response = this->StartNotificationResourceCreation(msg);

	else if (code == "load_texture_resource")
		response = this->LoadTextureResource(msg);

	else if (code == "load_font_resource")
		response = this->LoadFontResource(msg);

	else if (code == "add_rectangle_to_notification_resource")
		response = this->AddRectangleToNotificationResource(msg);

	else if (code == "add_circle_to_notification_resource")
		response = this->AddCircleToNotificationResource(msg);

	else if (code == "add_sprite_to_notification_resource")
		response = this->AddSpriteToNotificationResource(msg);

	else if (code == "add_text_to_notification_resource")
		response = this->AddTextToNotificationResource(msg);

	else if (code == "end_notification_resource_creation")
		response = this->EndNotificationResourceCreation(msg);

	else if (code == "pop_up")
		response = this->PopUpNotification(msg);

	else if (code == "close_daemon")
		response = this->CloseDaemon();

	else
		strcpy(sendBuffer, "{\"return_status\":\"failed\"}");//if the code is none one of the known

	if(response != "")
		strcpy(sendBuffer, response.c_str());

	thread.Send(sendBuffer);//send response
	free(sendBuffer);
}

std::string Api::StartNotificationResourceCreation(std::string msg)
{
	bool errors = false;
	//if the previus notification isn't assigned to any resource delete it
	if (tempNotificationPtr != nullptr)
		delete tempNotificationPtr;

	if (!GetValueFromMessage(msg, "name", tempNotififcatonBluePrintName))
		errors = true;

	tempNotificationPtr = new Notification();
	tempNotificationPtr->SetAnimation(new BaseAnimationFunction());
	sf::Vector2f position;
	sf::Vector2i size, window_position;

	if (!GetValueFromMessage(msg, "position.x", position.x))
		errors = true;

	if (!GetValueFromMessage(msg, "position.y", position.y))
		errors = true;

	if (!GetValueFromMessage(msg, "size.x", size.x))
		errors = true;

	if (!GetValueFromMessage(msg, "size.y", size.y))
		errors = true;

	if (!GetValueFromMessage(msg, "window_position.x", window_position.x))
		errors = true;

	if (!GetValueFromMessage(msg, "window_position.y", window_position.y))
		errors = true;

	tempNotificationPtr->SetPosition(position);
	tempNotificationPtr->SetWindowPosition(window_position);
	tempNotificationPtr->SetSize(size);

	//response
	if (!errors)
		return"{\"return_status\":\"done\"}";
	else
		return"{\"return_status\":\"failed\"}";
}

std::string Api::EndNotificationResourceCreation(std::string msg)
{
	auto resource = std::make_shared<NotificationBluePrintResource>(tempNotififcatonBluePrintName);
	resource->LoadFromNotification(tempNotificationPtr);
	resourceManagerInstance->GetListByName("NotificationBluePrints")->AddResource(resource);
	tempNotificationPtr = nullptr;//the owner of tempNotificationPtr is the new resource
	return "{\"return_status\":\"done\"}";
}

std::string Api::LoadTextureResource(std::string msg)
{
	bool errors = false;
	std::string path;
	if (!GetValueFromMessage(msg, "path", path))
		errors = true;

	std::string name;
	if (!GetValueFromMessage(msg, "name", name))
		errors = true;

	auto resource = std::make_shared<TextureResource>(name);
	if (!resource->LoadFromFile(path))
		errors = true;

	resourceManagerInstance->GetListByName("Textures")->AddResource(resource);

	if (!errors)
		return "{\"return_status\":\"done\"}";
	else
		return "{\"return_status\":\"failed\"}";
}

std::string Api::LoadFontResource(std::string msg)
{
	bool errors = false;
	std::string path;
	if (!GetValueFromMessage(msg, "path", path))
		errors = true;

	std::string name;
	if (!GetValueFromMessage(msg, "name", name))
		errors = true;

	auto resource = std::make_shared<FontResource>(name);
	if (!resource->LoadFromFile(path))
		errors = true;

	resourceManagerInstance->GetListByName("Fonts")->AddResource(resource);

	if (!errors)
		return "{\"return_status\":\"done\"}";
	else
		return "{\"return_status\":\"failed\"}";
}

std::string Api::AddRectangleToNotificationResource(std::string msg)
{
	bool errors = false;
	auto rectangle = new RectangleObject();

	sf::Vector2f position;
	if (!GetValueFromMessage(msg, "position.x", position.x))
		errors = true;
	if (!GetValueFromMessage(msg, "position.y", position.y))
		errors = true;

	sf::Vector2f size;
	if (!GetValueFromMessage(msg, "size.x", size.x))
		errors = true;
	if (!GetValueFromMessage(msg, "size.y", size.y))
		errors = true;

	sf::Color fill_color;
	unsigned int rgb = 0;
	if (!GetValueFromMessage(msg, "fill_color.r", rgb))
		errors = true;
	fill_color.r = rgb;
	if (!GetValueFromMessage(msg, "fill_color.g", rgb))
		errors = true;
	fill_color.g = rgb;
	if (!GetValueFromMessage(msg, "fill_color.b", rgb))
		errors = true;
	fill_color.b = rgb;

	sf::Color outline_color;
	if (!GetValueFromMessage(msg, "outline_color.r", rgb))
		errors = true;
	outline_color.r = rgb;
	if (!GetValueFromMessage(msg, "outline_color.g", rgb))
		errors = true;
	outline_color.g = rgb;
	if (!GetValueFromMessage(msg, "outline_color.b", rgb))
		errors = true;
	outline_color.b = rgb;

	float outline_thickness;
	if (!GetValueFromMessage(msg, "outline_thickness", outline_thickness))
		errors = true;

	std::string name;
	if (!GetValueFromMessage(msg, "name", name))
		errors = true;

	rectangle->SetSize(size);
	rectangle->SetFillColor(fill_color);
	rectangle->SetOutLineColor(outline_color);
	rectangle->SetOutLineThickness(outline_thickness);
	rectangle->SetName(name);
	tempNotificationPtr->GetBaseLayer()->AddObject(rectangle, position);

	if (!errors)
		return "{\"return_status\":\"done\"}";
	else
		return "{\"return_status\":\"failed\"}";
}

std::string Api::AddCircleToNotificationResource(std::string msg)
{
	bool errors = false;
	auto circle = new CircleObject();

	sf::Vector2f position;
	if (!GetValueFromMessage(msg, "position.x", position.x))
		errors = true;
	if (!GetValueFromMessage(msg, "position.y", position.y))
		errors = true;

	float size;
	if (!GetValueFromMessage(msg, "size", size))
		errors = true;

	sf::Color fill_color;
	unsigned int rgb = 0;
	if (!GetValueFromMessage(msg, "fill_color.r", rgb))
		errors = true;
	fill_color.r = rgb;
	if (!GetValueFromMessage(msg, "fill_color.g", rgb))
		errors = true;
	fill_color.g = rgb;
	if (!GetValueFromMessage(msg, "fill_color.b", rgb))
		errors = true;
	fill_color.b = rgb;

	sf::Color outline_color;
	if (!GetValueFromMessage(msg, "outline_color.r", rgb))
		errors = true;
	outline_color.r = rgb;
	if (!GetValueFromMessage(msg, "outline_color.g", rgb))
		errors = true;
	outline_color.g = rgb;
	if (!GetValueFromMessage(msg, "outline_color.b", rgb))
		errors = true;
	outline_color.b = rgb;

	float outline_thickness;
	if (!GetValueFromMessage(msg, "outline_thickness", outline_thickness))
		errors = true;

	std::string name;
	if (!GetValueFromMessage(msg, "name", name))
		errors = true;

	circle->SetSize(size);
	circle->SetFillColor(fill_color);
	circle->SetOutLineColor(outline_color);
	circle->SetOutLineThickness(outline_thickness);
	circle->SetName(name);
	tempNotificationPtr->GetBaseLayer()->AddObject(circle, position);

	if (!errors)
		return "{\"return_status\":\"done\"}";
	else
		return "{\"return_status\":\"failed\"}";
}

std::string Api::AddSpriteToNotificationResource(std::string msg)
{
	bool errors = false;
	sf::Vector2f position;
	if (!GetValueFromMessage(msg, "position.x", position.x))
		errors = true;
	if (!GetValueFromMessage(msg, "position.y", position.y))
		errors = true;

	sf::Vector2f size;
	if (!GetValueFromMessage(msg, "size.x", size.x))
		errors = true;
	if (!GetValueFromMessage(msg, "size.y", size.y))
		errors = true;

	sf::Color color;
	unsigned int rgb = 0;
	if (!GetValueFromMessage(msg, "color.r", rgb))
		errors = true;
	color.r = rgb;
	if (!GetValueFromMessage(msg, "color.g", rgb))
		errors = true;
	color.g = rgb;
	if (!GetValueFromMessage(msg, "color.b", rgb))
		errors = true;
	color.b = rgb;

	std::string name;
	if (!GetValueFromMessage(msg, "name", name))
		errors = true;

	std::string texture_name;
	if (!GetValueFromMessage(msg, "texture_name", texture_name))
		errors = true;

	auto resourcePtr = std::static_pointer_cast<TextureResource>(resourceManagerInstance->GetListByName("Textures")->SearchResource(texture_name));
	if (resourcePtr == nullptr)
		errors = true;
	else {
		sf::Texture* texture = resourcePtr->GetTexture();
		if (texture == nullptr) {
			errors = true;
			return "{\"return_status\":\"failed\"}";
		}
		auto sprite = new SpriteObject(texture);
		/*calculate scale for have the right size*/
		float scaleY = size.y / texture->getSize().y;
		float scaleX = size.x / texture->getSize().x;
		sprite->GetTransformableObject()->setScale(sf::Vector2f(scaleX, scaleY));
		sprite->SetColor(color);
		sprite->SetName(name);
		tempNotificationPtr->GetBaseLayer()->AddObject(sprite, position);
	}

	if (!errors)
		return "{\"return_status\":\"done\"}";
	else
		return "{\"return_status\":\"failed\"}";
}

std::string Api::AddTextToNotificationResource(std::string msg)
{
	bool errors = false;

	sf::Vector2f position;
	if (!GetValueFromMessage(msg, "position.x", position.x))
		errors = true;
	if (!GetValueFromMessage(msg, "position.y", position.y))
		errors = true;

	float character_size;
	if (!GetValueFromMessage(msg, "character_size", character_size))
		errors = true;

	sf::Color fill_color;
	unsigned int rgb = 0;
	if (!GetValueFromMessage(msg, "fill_color.r", rgb))
		errors = true;
	fill_color.r = rgb;
	if (!GetValueFromMessage(msg, "fill_color.g", rgb))
		errors = true;
	fill_color.g = rgb;
	if (!GetValueFromMessage(msg, "fill_color.b", rgb))
		errors = true;
	fill_color.b = rgb;

	sf::Color outline_color;
	if (!GetValueFromMessage(msg, "outline_color.r", rgb))
		errors = true;
	outline_color.r = rgb;
	if (!GetValueFromMessage(msg, "outline_color.g", rgb))
		errors = true;
	outline_color.g = rgb;
	if (!GetValueFromMessage(msg, "outline_color.b", rgb))
		errors = true;
	outline_color.b = rgb;

	float outline_thickness;
	if (!GetValueFromMessage(msg, "outline_thickness", outline_thickness))
		errors = true;

	std::string name;
	if (!GetValueFromMessage(msg, "name", name))
		errors = true;

	std::string string;
	if (!GetValueFromMessage(msg, "string", string))
		errors = true;

	std::string font_name;
	if (!GetValueFromMessage(msg, "font_name", font_name))
		errors = true;

	auto resourcePtr = std::static_pointer_cast<FontResource>(resourceManagerInstance->GetListByName("Fonts")->SearchResource(font_name));
	if (resourcePtr == nullptr)
		errors = true;
	else {
		sf::Font* font = resourcePtr->GetFont();
		if (font == nullptr) {
			errors = true;
			return "{\"return_status\":\"failed\"}";
		}
		auto text = new TextObject(font);
		text->SetCharacterSize(character_size);
		text->SetFillColor(fill_color);
		text->SetName(name);
		//text->SetOutLineColor(outline_color);
		//text->SetOutLineThickness(outline_thickness);
		text->SetString(string);
		tempNotificationPtr->GetBaseLayer()->AddObject(text, position);
	}
	if (!errors)
		return "{\"return_status\":\"done\"}";
	else
		return "{\"return_status\":\"failed\"}";
}

std::string Api::PopUpNotification(std::string msg)
{
	bool errors = false;
	std::string name = "";
	if (!GetValueFromMessage(msg, "name", name))
		errors = true;

	NotificationBluePrintResource* notificationBluePrint = static_cast<NotificationBluePrintResource*>(resourceManagerInstance->GetListByName("NotificationBluePrints")->SearchResource(name).get());

	if (notificationBluePrint == nullptr)
		errors = true;
	else
		renderingThread->PlayNotification(notificationBluePrint->GetNotificationCopy());

	if (!errors)
		return "{\"return_status\":\"done\"}";
	else
		return "{\"return_status\":\"failed\"}";
}

std::string Api::CloseDaemon()
{
	auto r = this->renderingThread;
	auto t = std::async(std::launch::async, [=] { Sleep(1000); r->CloseThread(); });//launch async so the socket can send the response
	return "{\"return_status\":\"done\"}";
}
