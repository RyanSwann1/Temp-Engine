#include "AnimationBase.h"
#include "SpriteSheet.h"


AnimationBase::AnimationBase() 
	:m_frameCurrent(0),
	m_frameTime(0),
	m_elaspedTime(0),
	m_frameActionStart(-1),
	m_frameActionEnd(-1),
	m_loop(false),
	m_playing(false)
{
}


AnimationBase::~AnimationBase()
{
}

void AnimationBase::setFrame(const unsigned int frame)
{
	//Check if new frame is within bounds of frames and to allow for frames that can play backwards
	if (frame >= m_frameActionStart && frame <= m_frameEnd &&
		frame >= m_frameEnd && frame <= m_frameStart) 
	{
		m_frameCurrent = frame;
	}
}

bool AnimationBase::isInAction()
{
	//If any action variables are -1, 'action' is always performed
	if (m_frameActionStart == -1 || m_frameActionEnd == -1) {
		return true;
	}
	//Current frame will be checked within the range that'll be loaded from the sprite sheet file
	return (m_frameCurrent >= m_frameActionStart && m_frameCurrent <= m_frameActionEnd);
}

void AnimationBase::setSpriteSheet(SpriteSheet* spriteSheet)
{
	m_spriteSheet = spriteSheet;
}

void AnimationBase::reset()
{
	//Reset the animation
	m_frameCurrent = m_frameStart;
	m_elaspedTime = 0;
	cropSprite();
}

void AnimationBase::update(const float deltaTime)
{
	if (m_playing)
	{
		m_elaspedTime += deltaTime;
		//Timer has exceeded the allowed frame time
		if (m_elaspedTime > m_frameTime) 
		{
			frameStep();
			cropSprite();
			m_elaspedTime = 0;
		}
	}
}
