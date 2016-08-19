#ifndef CHARACTER_H
#define CHARACTER_H

#include "EntityBase.h"
#include "Direction.h"
#include "SpriteSheet.h"

class Character :
	public EntityBase
{
public:
	Character(EntityManager* entityManager);
	virtual ~Character();

	void move(const Direction direction);
	void jump();
	void attack();
	void getHurt(const int damage);
	void load(const std::string& path);

	virtual void onEntityCollision(EntityBase* entity, const bool attack) = 0;
	virtual void update(float deltaTime); //Classes either will define own update method or extend this one
	void draw(sf::RenderWindow& window);

protected:
	void updateAttackAABB();
	void animate();
	SpriteSheet m_spriteSheet; //Have this in order to support animations

	float m_jumpVelocity;
	unsigned int m_hitPoints;
	sf::FloatRect m_attackAABB;
	sf::Vector2f m_attackAABBOffset;
};
#endif // !CHARACTER_H
