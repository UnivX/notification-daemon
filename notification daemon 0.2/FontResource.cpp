#include "FontResource.h"



FontResource::FontResource(std::string name)
{
	this->SetName(name);
	this->loaded = false;
}


bool FontResource::LoadFromFile(std::string path)
{
	if (this->texture.loadFromFile(path)) {
		this->loaded = true;
		return true;
	}
	return false;
}

FontResource::~FontResource()
{
}

sf::Font* FontResource::GetFont()
{
	if (this->loaded)
		return &this->texture;
	return nullptr;
}