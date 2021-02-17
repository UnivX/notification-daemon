#include "Animations.h"
#define MULT 0.2f
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
		velocity = ((position.x / MULT) + 0.1);
		if (velocity > 25)
			velocity = 25;
		if (position.x > 0) {
			position.x -= velocity * deltaTime * 30;
		}
		else
		{
			position.x = 0;
			fase = false;
		}
	}
	else
	{
		if (timer < 6) {
			timer += deltaTime;
		}
		else {
			velocity = ((position.x / MULT) + 0.1);
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
	if (position.x == float(-int(0xffffffff)) || position.x == float(int(0xffffffff)))
		ended = true;
}