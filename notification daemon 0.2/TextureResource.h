#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include <atomic>
/*
this class own the texture resource
*/
class TextureResource : public Resource
{
public:
	bool LoadFromFile(std::string path);
	TextureResource(std::string name);
	~TextureResource();
	sf::Texture* GetTexture();
private:
	sf::Texture texture;
	std::atomic<bool> loaded;//is atomi cso i dont need mutex to be thread safe
};

