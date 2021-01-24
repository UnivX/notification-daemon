#include "RenderingObject.h"

RenderingObject::RenderingObject()
{
}

RenderingObject::~RenderingObject()
{
}

std::string RenderingObject::GetName()
{
	return this->name;
}

void RenderingObject::SetName(std::string name)
{
	this->name = name;
}


SpriteObject::SpriteObject(sf::Texture* texture)
{
	this->sprite.setTexture(*texture);
}

sf::Drawable* SpriteObject::GetDrawableObject()
{
	return &this->sprite;
}

sf::Transformable* SpriteObject::GetTransformableObject()
{
	return &this->sprite;
}

void SpriteObject::SetColor(sf::Color color)
{
	this->sprite.setColor(color);
}

TextObject::TextObject(sf::Font* font)
{
	this->text.setFont(*font);
}

sf::Drawable* TextObject::GetDrawableObject()
{
	return &this->text;
}

sf::Transformable* TextObject::GetTransformableObject()
{
	return &this->text;
}

void TextObject::SetCharacterSize(unsigned int size)
{
	this->text.setCharacterSize(size);
}

void TextObject::SetString(std::string str)
{
	this->text.setString(str);
}

void TextObject::SetOutLineThickness(float thickness)
{
	this->text.setOutlineThickness(thickness);
}

void TextObject::SetFillColor(sf::Color color)
{
	this->text.setFillColor(color);
}

void TextObject::SetOutLineColor(sf::Color color)
{
	this->text.setOutlineColor(color);
}

RectangleObject::RectangleObject()
{
}

sf::Drawable* RectangleObject::GetDrawableObject()
{
	return &this->shape;
}

sf::Transformable* RectangleObject::GetTransformableObject()
{
	return &this->shape;
}

void RectangleObject::SetSize(sf::Vector2f size)
{
	this->shape.setSize(size);
}

void RectangleObject::SetOutLineThickness(float thickness)
{
	this->shape.setOutlineThickness(thickness);
}

void RectangleObject::SetFillColor(sf::Color color)
{
	this->shape.setFillColor(color);
}

void RectangleObject::SetOutLineColor(sf::Color color)
{
	this->shape.setOutlineColor(color);
}

CircleObject::CircleObject()
{
}

sf::Drawable* CircleObject::GetDrawableObject()
{
	return &this->shape;
}

sf::Transformable* CircleObject::GetTransformableObject()
{
	return &this->shape;
}

void CircleObject::SetSize(float radius)
{
	this->shape.setRadius(radius);
	float r = this->shape.getRadius();
	return;
}

void CircleObject::SetOutLineThickness(float thickness)
{
	this->shape.setOutlineThickness(thickness);
}

void CircleObject::SetFillColor(sf::Color color)
{
	this->shape.setFillColor(color);
}

void CircleObject::SetOutLineColor(sf::Color color)
{
	this->shape.setOutlineColor(color);
}

Layer::Layer()
{
	this->position = sf::Vector2f(0, 0);
}

void Layer::UpdatePosition()
{
	for (int i = 0; i < this->vector.size(); i++) {
		this->vector[i].first->GetTransformableObject()->setPosition(this->GetPosition() + this->vector[i].second);
	}
}

void Layer::AddObject(RenderingObject* object, sf::Vector2f localPosition)
{
	this->vector.push_back(std::pair<RenderingObject*, sf::Vector2f>(object, localPosition));
}

void Layer::SetObjectLocalPosition(RenderingObject* object, sf::Vector2f localPosition)
{
	//search and change position
	for (int i = 0; i < this->vector.size(); i++) {
		if (this->vector[i].first == object) {
			this->vector[i].second = localPosition;
			break;
		}
	}
}

sf::Vector2f Layer::GetObjectLocalPosition(RenderingObject* object)
{
	//search and change position
	for (int i = 0; i < this->vector.size(); i++) {
		if (this->vector[i].first == object)
			return this->vector[i].second;
	}
	return sf::Vector2f();
}

sf::Vector2f Layer::GetPosition()
{
	return this->position;
}

void Layer::SetPosition(sf::Vector2f position)
{
	this->position = position;
}

std::vector<std::pair<RenderingObject*, sf::Vector2f>>* Layer::GetVector()
{
	return &this->vector;
}
