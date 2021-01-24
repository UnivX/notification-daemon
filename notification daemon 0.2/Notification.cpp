#include "Notification.h"



Notification::Notification()
{
	this->ended = false;
	this->position = sf::Vector2f(0, 0);
}


Notification::~Notification()
{
	//free all the rendering objects
	auto localVector = this->baseLayer.GetVector();
	for (int i = 0; i < localVector->size(); i++)
		delete (*localVector)[i].first;
	if (this->animationFunction != nullptr)
		delete this->animationFunction;
}

Layer* Notification::GetBaseLayer()
{
	return &this->baseLayer;
}

void Notification::RenderAll(OverlayWindow& window)
{
	//for all the objects in the layer draw to the window
	auto vector = this->baseLayer.GetVector();
	for (int i = 0; i < vector->size(); i++)
		window.draw(*(*vector)[i].first->GetDrawableObject());
}

void Notification::Update(float deltaTime)
{
	this->baseLayer.SetPosition(this->position);
	this->baseLayer.UpdatePosition();
	this->animationFunction->Function(this->position, this->ended, deltaTime);
}

bool Notification::IsEnded()
{
	return this->ended;
}

Notification* Notification::CreateDynamicCopy()
{
	auto notification = new Notification(*this);
	notification->ResetEnded();
	notification->SetAnimation(this->animationFunction->CreateDynamicCopy());
	auto targetVector = notification->GetBaseLayer()->GetVector();
	auto localVector = this->baseLayer.GetVector();
	targetVector->clear();//clear the vector from the local pointers

	for (int i = 0; i < localVector->size(); i++)
		targetVector->emplace_back(std::pair<RenderingObject*, sf::Vector2f>((*localVector)[i].first->CreateDynamicCopy(), (*localVector)[i].second));

	return notification;
}

void Notification::SetAnimation(AnimationFunction* animationFunction)
{
	this->animationFunction = animationFunction;
}

void Notification::SetPosition(sf::Vector2f position)
{
	this->position = position;
}

void Notification::SetSize(sf::Vector2i size)
{
	this->size = size;
}

sf::Vector2i Notification::GetSize()
{
	return this->size;
}

void Notification::SetWindowPosition(sf::Vector2i pos)
{
	this->windowPosition = pos;
}

sf::Vector2i Notification::GetWindowPosition()
{
	return this->windowPosition;
}

void Notification::ResetEnded()
{
	this->ended = false;
}
