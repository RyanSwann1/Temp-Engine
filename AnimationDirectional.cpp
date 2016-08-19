#include "AnimationDirectional.h"
#include "SpriteSheet.h"


void AnimationDirectional::cropSprite()
{
	//Get the position of the sprite in relation to which frame were on and which row were on too
	sf::IntRect rect;
	//Assign the locatuion of the sprite on the sprite sheet
	rect.left = m_spriteSheet->getSpriteSize().x * m_frameCurrent;
	rect.top = m_spriteSheet->getSpriteSize().y * (m_frameRow + (short)m_spriteSheet->getDirection()); 
	//Assign the size of the sprite
	rect.width = m_spriteSheet->getSpriteSize().x;
	rect.height = m_spriteSheet->getSpriteSize().y;
}

void AnimationDirectional::frameStep()
{
	//Control in which direction frames move 
	if (m_frameStart < m_frameEnd) {
		m_frameCurrent++;
	}
	else {
		m_frameCurrent--;
	}
	
	//Check whether the frame is out of bounds
	if(m_frameStart < m_frameEnd && m_frameCurrent > m_frameEnd || 
		m_frameStart > m_frameEnd && m_frameCurrent < m_frameEnd)
	{
		//Reset 
		if (m_loop) {
			m_frameCurrent = m_frameStart; 
			return;
		}
		//Re crop
		m_frameCurrent = m_frameEnd;
		pause();
	}
}

void AnimationDirectional::readIn(std::stringstream & keyStream)
{
	//Read in data for variables from file
	keyStream >> m_frameStart >> m_frameEnd >> m_frameRow 
		>> m_frameTime >> m_frameActionStart >> m_frameActionEnd;
}
