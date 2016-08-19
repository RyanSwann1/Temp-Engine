#include "Character.h"
#include "EntityManager.h"
#include "Utilities.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>


Character::Character(EntityManager* entityManager)
	: EntityBase(entityManager),
	m_spriteSheet(*m_entityManager->getContext().m_textureManager),
	m_jumpVelocity(250),
	m_hitPoints(5)
{
	m_name = "Character";
}


Character::~Character()
{
}

void Character::move(const Direction direction)
{
	//Cannot move when dying/dead
	if (getState() == EntityState::Dying) {
		return;
	}
	m_spriteSheet.setDirection(direction);
	//Move Right
	if (direction == Direction::RIGHT) {
		accelerate(m_speed.x, 0);
	}
	//Move left
	else {
		accelerate(-m_speed.x, 0);
	}
	//if character idling, set to walking
	if (getState() == EntityState::Idle) {
		setState(EntityState::Walking);
	}
}

void Character::jump()
{
	//Cannot jump when in states
	if (getState() == EntityState::Jumping ||
		getState() == EntityState::Dying ||
		getState() == EntityState::Attacking) {
		return;
	}
	//Apply jump
	setState(EntityState::Jumping);
	addVelocity(0, -m_jumpVelocity);
}

void Character::attack()
{
	//Cannot attack when in states
	if (getState() == EntityState::Dying) {
		return;
	}
	//Apply attack
	setState(EntityState::Attacking);
}

void Character::getHurt(const int damage)
{
	//Cannot receive damage in states
	if (getState() == EntityState::Dying ||
		getState() == EntityState::Hurt) {
		return;
	}
	//Take away hitPoitns in relation to dmg
	m_hitPoints = (m_hitPoints - damage > 0 ? m_hitPoints - damage : 0);
	if (m_hitPoints) {
		setState(EntityState::Hurt);
	}
	else {
		setState(EntityState::Dying);
	}
}

void Character::load(const std::string & path)
{
	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + path);
	if (!file.is_open()) {
		std::cerr << "Unable to load file: " << path << "\n";
		return;
	}
	else
	{
		std::string line;
		while (std::getline(file, line))
		{
			std::stringstream keyStream(line);
			std::string type;
			keyStream >> type;
			if (type == "Name") {
				keyStream >> m_name;
			}
			else if (type == "Spritesheet")
			{
				std::string path;
				keyStream >> path;
				m_spriteSheet.loadSheet("media/SpriteSheet/" + path);
			}
			else if (type == "Hitpoints") {
				keyStream >> m_hitPoints;
			}
			else if (type == "BoundingBox")
			{
				sf::Vector2f boundingBox;
				keyStream >> boundingBox.x >> boundingBox.y;
				setSize(boundingBox.x, boundingBox.y);
			}
			else if (type == "DamageBox")
			{
				keyStream >> m_attackAABBOffset.x >> m_attackAABBOffset.y >>
					m_attackAABB.width >> m_attackAABB.height;
			}
			else if (type == "Speed") {
				keyStream >> m_speed.x >> m_speed.y;
			}
			else if (type == "JumpVelocity") {
				keyStream >> m_jumpVelocity;
			}
			else if (type == "MaxVelocity") {
				keyStream >> m_maxVelocity.x >> m_maxVelocity.y;
			}
			else {
				std::cerr << "Unknown type in character file: " << type << "\n";
			}
		}
		file.close();
	}
}

void Character::update(float deltaTime)
{
	EntityBase::update(deltaTime);
	//If Attack AABB is in use - update
	if (m_attackAABB.width != 0 && m_attackAABB.height != 0) {
		updateAttackAABB();
	}

	//Handle state transitions

	//If state not dying, attacking nor being damaged. 
	//Current state will be determined by its velocity
	EntityState state = getState();
	if (state != EntityState::Dying && state != EntityState::Attacking && state != EntityState::Hurt)
	{
		//handles state transitions
		if (m_velocity.y >= 0.001) {
			setState(EntityState::Jumping);
		}
		else if (abs(m_velocity.x) >= 0.1f) {
			setState(EntityState::Walking);
		}
		else {
			setState(EntityState::Idle);
		}
	}
	//Because the attacking and hurt states aren't set to loop, the sprite animation is checked to see if it is still playing
	else if (state == EntityState::Attacking || state == EntityState::Hurt)
	{
		//After the particular animation has finished, set to idle - due to not looping. Animation only plays once
		if (!m_spriteSheet.getCurrentAnimation()->isPlaying()) {
			setState(EntityState::Idle);
		}
	}
	//if entity state is set to dying
	else if (state == EntityState::Dying)
	{
		//After the dying animation has finished
		if (!m_spriteSheet.getCurrentAnimation()->isPlaying()) {
			m_entityManager->remove(m_id); //Remove entity from game
		}
	}
	//Animate method is called late to reflect the changes made to the state
	animate(); //Animate entity
	m_spriteSheet.update(deltaTime); //Update the sprite sheet
 	m_spriteSheet.setSpritePos(m_position); //Set the sprite pos
}

void Character::draw(sf::RenderWindow & window)
{
	//The spritesheet handles the drawing aspect
	m_spriteSheet.draw(window);
}

void Character::updateAttackAABB()
{
	////Reposition the attack bounding box of the character
	//m_attackAABB.left =
	//	(m_spriteSheet.getDirection() == Direction::LEFT ? //if direction left
	//	(m_AABB.left - m_attackAABB.width) - m_attackAABBOffset.x //Assign position of the bounding box to the left of the entity
	//		: (m_AABB.left + m_attackAABB.width) + m_attackAABBOffset.x); //Else, assign to the right of the entity
	//
	//m_attackAABB.top = m_AABB.top + m_attackAABBOffset.y;

	//Reposition the attack bounding box
	//Assign left position
	m_attackAABB.left = (m_spriteSheet.getDirection() == Direction::LEFT ? //If sprite sheet direction is left
		(m_AABB.left - m_attackAABB.width) - m_attackAABBOffset.x //Assign to left position
		: (m_AABB.left + m_attackAABB.width) + m_attackAABBOffset.x); //Assign to the right position

	//Assign the top position
	m_attackAABB.top = m_AABB.top + m_attackAABBOffset.y;
}

void Character::animate()
{
	//Get the current state 
	EntityState state = getState();

	//Check the current state & current animation match
	//If not rectify - 
	if (state == EntityState::Walking && m_spriteSheet.getCurrentAnimation()->getName() != "Walk") {
		m_spriteSheet.setAnimation("Walk", true, true);
	}
	else if (state == EntityState::Jumping && m_spriteSheet.getCurrentAnimation()->getName() != "Jump") {
		m_spriteSheet.setAnimation("Jump", true, false);
	}
	else if (state == EntityState::Attacking && m_spriteSheet.getCurrentAnimation()->getName() != "Attack") {
		m_spriteSheet.setAnimation("Attack", true, false);
	}
	else if (state == EntityState::Hurt && m_spriteSheet.getCurrentAnimation()->getName() != "Hurt") {
		m_spriteSheet.setAnimation("Hurt", true, false);
	}
	else if (state == EntityState::Dying && m_spriteSheet.getCurrentAnimation()->getName() != "Dying") {
		m_spriteSheet.setAnimation("Dying", true, false);
	}
	else if (state == EntityState::Idle && m_spriteSheet.getCurrentAnimation()->getName() != "Idle") {
		m_spriteSheet.setAnimation("Idle", true, true);
	}
}
