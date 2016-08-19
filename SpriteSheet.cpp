#include "SpriteSheet.h"
#include "Utilities.h"
#include <fstream>
#include <sstream>



SpriteSheet::SpriteSheet(TextureManager& textureManager)
	: m_textureManager(textureManager),
	m_animationCurrent(nullptr),
	m_spriteScale(1.0f, 1.0f),
	m_directionCurrent(Direction::RIGHT)
{
	registerAnimations<AnimationDirectional>("Directional");
}


SpriteSheet::~SpriteSheet()
{
	//Release the resource for the spritesheet that is currently in use.
	releaseSheet();
	delete m_animationCurrent;
	m_animationCurrent = nullptr;
}

void SpriteSheet::releaseSheet()
{
	m_textureManager.releaseResource(m_textureName);
	for (auto &i : m_animations)
	{
		delete i.second;
		i.second = nullptr;
	}
	m_animations.clear();
}

inline void SpriteSheet::setSpriteSize(const sf::Vector2i & size)
{
	//Update the sprites origin after changing it's position
	m_spriteSize = size;
	m_sprite.setOrigin(m_spriteSize.x / 2.0f, m_spriteSize.y / 2.0f);
}

void SpriteSheet::setAnimation(const std::string & name, const bool play, const bool loop)
{

	//Set a new animation
	auto iter = m_animations.find(name);
	if (iter != m_animations.end() && m_animationCurrent != iter->second)
	{
		//Stop the current animation
		if (m_animationCurrent) {
			m_animationCurrent->stop();
		}
		//Assign the new animation
		m_animationCurrent = iter->second;
		m_animationCurrent->setLooping(loop);
		if (play) {
			m_animationCurrent->play();
		}
		//Assign the new sprite
		m_animationCurrent->cropSprite();
	}
}

void SpriteSheet::setDirection(const Direction direction)
{
	if (m_directionCurrent != direction)
	{
		m_directionCurrent = direction;
		m_animationCurrent->cropSprite();
	}
}

bool SpriteSheet::loadSheet(const std::string & file)
{
	std::ifstream sheet;
	sheet.open(Utils::GetWorkingDirectory() + file);
	if (sheet.is_open())
	{
		std::string line;
		while (std::getline(sheet, line))
		{
			//Redundant line, move onto next
			if (line[0]) {
				continue;
			}
			std::stringstream keyStream(line);
			std::string type;
			keyStream >> type;
			if (type == "Texture")
			{
				//Duplicate texture found, move onto next line
				if (m_textureName != "") {
					std::cerr << "Duplicate texture entries in:" << file << "\n";
					continue;
				}

				std::string texture;
				keyStream >> texture;
				//Attempt to load in the sprite sheet 
				if (!m_textureManager.requireResource(texture))
				{
					//couldnt load in texture
					std::cerr << "Couldn't fetch texture: " << texture << "\n";
					continue;
				}
				m_textureName = texture; //Assigned name so later can free the resource.
				//Without name, couldn't free the spritesheet resource - thus memory leak when not used
				m_sprite.setTexture(*m_textureManager.getResource(m_textureName));
			}
			else if (type == "Size") {
				keyStream >> m_spriteSize.x >> m_spriteSize.y;
				setSpriteSize(m_spriteSize);
			}
			else if (type == "Scale") {
				keyStream >> m_spriteScale.x >> m_spriteScale.y;
				m_sprite.setScale(m_spriteScale);
			}
			else if (type == "AnimationType") {
				keyStream >> m_animationType;
			}
			else if (type == "Animation")
			{
				std::string name;
				keyStream >> name;
				if (m_animations.find(name) != m_animations.end())
				{
					std::cerr << "Duplicate animation found: " << name << "\n";
					continue;
				}

				AnimationBase* animation = nullptr;
				auto iter = m_animationFactory.find(m_animationType);
				if (iter != m_animationFactory.end()) {
					animation = iter->second;
				}
				else {
					std::cerr << "Unknown animation type: " << m_animationType << "\n";
					continue;
				}

				keyStream >> *animation;
				animation->setSpriteSheet(this);
				animation->setName(name);
				m_animations.emplace(name, animation);

				//If current animation already set, continue
				if (m_animationCurrent) {
					continue;
				}
				m_animationCurrent = animation;
				m_animationCurrent->play();

			}
		}
		//Sheet successfully opened
		sheet.close();
		return true;
	}
	//Sheet unsuccessfully opened
	return false;
}

void SpriteSheet::update(const float deltaTime)
{
	m_animationCurrent->update(deltaTime);
}

void SpriteSheet::draw(sf::RenderWindow & window)
{
	window.draw(m_sprite);
}
