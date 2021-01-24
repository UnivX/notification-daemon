#include "TextureResource.h"



TextureResource::TextureResource(std::string name)
{
	this->SetName(name);
	this->loaded = false;
}


bool TextureResource::LoadFromFile(std::string path)
{
	if (this->texture.loadFromFile(path)) {
		this->loaded = true;
		return true;
	}
	return false;
}

TextureResource::~TextureResource()
{
}

sf::Texture* TextureResource::GetTexture()
{
	if (this->loaded)
		return &this->texture;
	return nullptr;
}
