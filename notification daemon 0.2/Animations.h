#pragma once
#include "Notification.h"

class BaseAnimationFunction : public AnimationFunction {
public:
	BaseAnimationFunction();
	AnimationFunction* CreateDynamicCopy();
	void Function(sf::Vector2f& position, bool& ended, float deltaTime);
private:
	float velocity;
	float timer;
	bool fase;
};