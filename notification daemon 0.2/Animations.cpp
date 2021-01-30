#include "Animations.h"

BaseAnimationFunction::BaseAnimationFunction() {
	this->velocity = 0;
	this->fase = true;
	this->timer = 0;
}


AnimationFunction* BaseAnimationFunction::CreateDynamicCopy() {
	return new BaseAnimationFunction();
}


void BaseAnimationFunction::Function(sf::Vector2f& position, bool& ended, float deltaTime) {
	if (fase) {
		velocity = ((position.x / 1.f) + 0.1);
		if (velocity > 25)
			velocity = 25;
		if (position.x >= 0) {
			position.x -= velocity * deltaTime * 30;
		}
		else
		{
			fase = false;
		}
	}
	else
	{
		if (timer < 2) {
			timer += deltaTime;
		}
		else {
			velocity = ((position.x) + 0.1);
			if (velocity > 25)
				velocity = 25;
			if (position.x <= 400) {
				position.x += velocity * deltaTime * 30;
			}
			else
			{
				ended = true;
			}
		}
	}
	if (position.x == -INFINITY || position.x == INFINITY)
		ended = true;
}