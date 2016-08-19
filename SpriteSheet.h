#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H

#include <SFML\Graphics.hpp>
#include "TextureManager.h"
#include "AnimationBase.h"
#include "AnimationDirectional.h"
#include "Direction.h"
#include <string>
#include <unordered_map>

using AnimationName = std::string;

class SpriteSheet
{
public:
	SpriteSheet(TextureManager& textureManager);
	~SpriteSheet();

	void releaseSheet();
	sf::Vector2i getSpriteSize() const { return m_spriteSize; }
	Direction getDirection() const { return m_directionCurrent; }
	AnimationBase* getCurrentAnimation() const { return m_animationCurrent; }

	inline void setSpriteSize(const sf::Vector2i& size);
	void setAnimation(const std::string& name, const bool play, const bool loop);
	void setSpritePos(const sf::Vector2f& pos) { m_sprite.setPosition(pos); }
	void setDirection(const Direction direction);
	bool loadSheet(const std::string& file);

	void update(const float deltaTime);
	void draw(sf::RenderWindow& window);

	template <class T>
	void registerAnimations(const AnimationName& animationName)
	{
		if (m_animationFactory.find(animationName) == m_animations.end())
		{
			m_animations[animationName] = new T;
		}
	}

private:
	std::unordered_map<AnimationName, AnimationBase*> m_animationFactory; //Stores all available animations
	std::unordered_map<AnimationName, AnimationBase*> m_animations;
	TextureManager m_textureManager;
	std::string m_textureName; //Sprite Sheet Name
	std::string m_animationType;
	sf::Vector2i m_spriteSize;
	sf::Vector2f m_spriteScale;
	sf::Sprite m_sprite; //The sprite sheet
	Direction m_directionCurrent;
	AnimationBase* m_animationCurrent; //Animation currently in use
};
#endif // !SPRITE_SHEET_H