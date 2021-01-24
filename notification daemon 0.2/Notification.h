#pragma once
#include "RenderingObject.h"
#include "OverlayWindow.h"
#include <vector>

class AnimationFunction {
public:
	virtual void Function(sf::Vector2f& position, bool& ended, float deltaTime) = 0;
	virtual AnimationFunction* CreateDynamicCopy() = 0;
};

/*
this class own the layer object and all the objects that there are attached to it
is possible do a deep copy of the class with the function CreateDynamicCopy()
TODO:
-create and implement the animation component
*/
class Notification
{
public:
	Notification();
	~Notification();
	Layer* GetBaseLayer();
	void RenderAll(OverlayWindow& window);
	void Update(float deltaTime);
	bool IsEnded();
	Notification* CreateDynamicCopy();//to test if it works
	void SetAnimation(AnimationFunction* animationFunction);
	void SetPosition(sf::Vector2f position);
	void SetSize(sf::Vector2i size);
	sf::Vector2i GetSize();
	void SetWindowPosition(sf::Vector2i pos);
	sf::Vector2i GetWindowPosition();
	void ResetEnded();
private:
	sf::Vector2i size;
	sf::Vector2f position;
	sf::Vector2i windowPosition;
	AnimationFunction* animationFunction;
	Layer baseLayer;
	bool ended;
};

