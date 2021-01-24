#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include <atomic>
/*
this is the class that own the font resource
*/
class FontResource : public Resource
{
public:
	bool LoadFromFile(std::string path);
	FontResource(std::string name);
	~FontResource();
	sf::Font* GetFont();
private:
	sf::Font texture;
	std::atomic<bool> loaded;//is atomic so i dont need mutex to be thread safe
};