#ifndef ANIMATION_DIRECTIONAL_H
#define ANIMATION_DIRECTIONAL_H


#include "AnimationBase.h"
class AnimationDirectional :
	public AnimationBase
{
public:

private:
	void frameStep() override;
	void cropSprite() override;
	void readIn(std::stringstream& keyStream) override;
};

#endif // !ANIMATION_DIRECTIONAL_H