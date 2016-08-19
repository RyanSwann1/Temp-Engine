#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#include <SFML\Graphics.hpp>
#include <string>
#include <vector>

struct CollisionElement
{
	CollisionElement(float area, TileInfo* tile, const sf::FloatRect& bounds)
		:m_area(area),
		m_tile(tile),
		m_tileBounds(bounds) {}

	float m_area;
	TileInfo* m_tile;
	sf::FloatRect m_tileBounds;
};

struct TileInfo;
class EntityManager;
class EntityBase
{
	friend class EntityManager;
public:
	EntityBase(EntityManager* entityManager);
	virtual ~EntityBase();
	
	std::string getName() const { return m_name; }
	EntityType getType() const { return m_type; }
	EntityState getState() const { return m_stateCurrent; }

	void setPosition(float x, float y);
	void setPosition(const sf::Vector2f& pos);
	void setSize(float x, float y);
	void setState(const EntityState state);
	
	void move(float x, float y);
	void addVelocity(float x, float y);
	void accelerate(float x, float y) { m_acceleration += sf::Vector2f(x, y); }
	void setAcceleration(float x, float y) { m_acceleration = sf::Vector2f(x, y); }
	void applyFriction(float x, float y);
	
	void update(const float deltaTime);
	virtual void draw(sf::RenderWindow& window) = 0;

protected:
	// Methods.
	inline void updateAABB();
	void checkCollisions();
	void resolveCollisions();
	bool sortCollisions(const CollisionElement& col1, const CollisionElement& col2) { return col1.m_area > col2.m_area; }

	// Method for what THIS entity does TO the l_collider entity.
	virtual void onEntityCollision(EntityBase* l_collider, bool l_attack) = 0;

	std::string m_name;
	EntityType m_type;
	unsigned int m_id; // Entity id in the entity manager.
	sf::Vector2f m_position; // Current position.
	sf::Vector2f m_positionOld; // Position before entity moved.
	sf::Vector2f m_velocity; // Current velocity.
	sf::Vector2f m_maxVelocity; // Maximum velocity.
	sf::Vector2f m_speed; // Value of acceleration.
	sf::Vector2f m_acceleration; // Current acceleration.
	sf::Vector2f m_friction; // Default friction value.
	TileInfo* m_referenceTile; // Tile underneath entity.
	sf::Vector2f m_size; // Size of the collision box.
	sf::FloatRect m_AABB; // The bounding box for collisions.
	EntityState m_stateCurrent; // Current entity state.
	
	// Flags for remembering axis collisions.
	bool m_collidingOnX;
	bool m_collidingOnY;
	std::vector<CollisionElement> m_collisions;
	EntityManager* m_entityManager;
};
#endif // !ENTITY_BASE_H
