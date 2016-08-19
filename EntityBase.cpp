#include "EntityBase.h"
#include "EntityManager.h"
#include "Map.h"
#include <math.h>

EntityBase::EntityBase(EntityManager* entityManager)
	: m_entityManager(entityManager),
	m_name("BaseEntity"),
	m_type(EntityType::Base),
	m_referenceTile(nullptr),
	m_stateCurrent(EntityState::Idle),
	m_id(0),
	m_collidingOnX(false),
	m_collidingOnY(false)
{
}

EntityBase::~EntityBase()
{
}

void EntityBase::setPosition(float x, float y)
{
	m_position = sf::Vector2f(x, y);
	updateAABB(); //Update bounding box in relation to change
}

void EntityBase::setPosition(const sf::Vector2f & pos)
{
	m_position = pos;
	updateAABB(); //Update bounding box in relation to change
}

void EntityBase::setSize(float x, float y)
{
	m_size = sf::Vector2f(x, y);
	updateAABB(); //Update bounding box in relation to change
}

void EntityBase::setState(const EntityState state)
{
	if (m_stateCurrent != EntityState::Dying) {
		m_stateCurrent = state;
	}
}

void EntityBase::move(float x, float y)
{
	m_positionOld = m_position; //Assign the old position before movement
	m_position += sf::Vector2f(x, y);

	//Compare current position to map size
	//Check x axis
	sf::Vector2u mapSize = m_entityManager->getContext().m_map->getMapSize();
	if (m_position.x < 0) {
		m_position.x = 0;
	}
	else if (m_position.x > (mapSize.x + 1) * TileSheet::TileSize) {
		m_position.x = (mapSize.x + 1) * TileSheet::TileSize;
	}

	//Check Y axis
	if (m_position.y < 0) {
		m_position.y = 0;
	}
	else if (m_position.y > (mapSize.y + 1) * TileSheet::TileSize) {
		m_position.y = (mapSize.y + 1) * TileSheet::TileSize;
		setState(EntityState::Dying); //Entity has exceeded bounds of the map
	}

	//Update the bounding box in order to reflect the changes made to entity
	updateAABB();
}

void EntityBase::addVelocity(float x, float y)
{
	m_velocity += sf::Vector2f(x, y);
	//Check that velocity is not exceeding maximum
	//X Axis
	if (abs(m_velocity.x) > m_maxVelocity.x)
	{
		if (m_velocity.x < 0) {
			m_velocity.x = -m_maxVelocity.x;
		}
		else {
			m_velocity.x = m_maxVelocity.x;
		}
	}

	//Y Axis
	else if (abs(m_velocity.y) > m_maxVelocity.y)
	{
		if (m_velocity.y < 0) {
			m_velocity.y = -m_maxVelocity.y;
		}
		else {
			m_velocity.y = m_maxVelocity.y;
		}
	}
}

void EntityBase::applyFriction(float x, float y)
{
	//X Axis
	if (m_velocity.x != 0)
	{
		//Do not allow change of velocity to be below that of minimum amount
		if (abs(m_velocity.x) - abs(x) < 0) {
			m_velocity.x = 0;
		}
		else 
		{
			//Reverse direction in which entity is moving
			if (m_velocity.x < 0) {
				m_velocity.x += x;
			}
			else {
				m_velocity.x -= x;
			}
		}
	}

	//Y Axis
	if (m_velocity.y)
	{
		if (abs(m_velocity.y) - abs(x) < 0) {
			m_velocity.y = 0;
		}
		else
		{
			//Reverse direction in which entity is moving
			if (m_velocity.y < 0) {
				m_velocity.y += y;
			}
			else {
				m_velocity.y -= y;
			}
		}
	}
}

void EntityBase::update(const float deltaTime)
{
	Map* map = m_entityManager->getContext().m_map;
	accelerate(0, map->getGravity()); //Apply gravity to entity
	addVelocity(m_acceleration.x * deltaTime, m_acceleration.y * deltaTime); //Add velocity to entity
	setAcceleration(0.0f, 0.0f);
	sf::Vector2f frictionValue;
	//If entity is standing on tile
	if (m_referenceTile)
	{
		frictionValue = m_referenceTile->m_friction; //Get reference tiles friction value
		if (m_referenceTile->m_deadly) {
			setState(EntityState::Dying);
		}
	}
	//If entity is in mid air - using default friction value
	else if (map->getDefaultTile()) { //Test for null - whether or not active
		frictionValue = map->getDefaultTile()->m_friction;
	}
	//Rely on the default set friction value
	else {
		frictionValue = m_friction;
	}

	//Apply friction - alter the speed of the entity with friction value
	float frictionX = (m_speed.x * frictionValue.x) * deltaTime;
	float frictionY = (m_speed.y * frictionValue.y) * deltaTime;
	applyFriction(frictionX, frictionY);
	//Apply velocity
	sf::Vector2f deltaPos = m_velocity * deltaTime; //Calculate velocity with delta time
	move(deltaPos.x, deltaPos.y); //Move entity
	//Reset collision Checks
	m_collidingOnX = false;
	m_collidingOnY = false;
	checkCollisions();
	resolveCollisions();
}

inline void EntityBase::updateAABB()
{
	m_AABB = sf::FloatRect(m_position.x - (m_size.x / 2.0f), m_position.y - m_size.y, m_size.x, m_size.y);
}

void EntityBase::checkCollisions()
{
	Map* map = m_entityManager->getContext().m_map;
	unsigned int tileSize = map->getTileSize();
	//Calculate intersecting points
	int fromX = floor(m_AABB.left / tileSize);
	int toX = floor((m_AABB.left + m_AABB.width) / tileSize);
	int fromY = floor(m_AABB.top / tileSize);
	int toY = floor((m_AABB.top + m_AABB.height) / tileSize);

	//Checks whether a tile is within the space were interestd in
	for (int x = fromX; x <= toX; x++)
	{
		for (int y = fromY; y <= toY; y++)
		{
			Tile* tile = map->getTile(x, y); //Get requested tile
			if (tile)
			{
				sf::FloatRect tileBounds(x * tileSize, y * tileSize, tileSize, tileSize);
				sf::FloatRect intersection;
				m_AABB.intersects(tileBounds, intersection);
				float area = intersection.height * intersection.width;

				//Store Collision
				CollisionElement collision(area, tile->m_tileInfo, tileBounds);
				m_collisions.push_back(collision);
				//If tile loasd next lvl and entity is player type
				if (tile->m_warp && m_type == EntityType::Player) {
					map->loadNextMap();
				}
			}
		}
	}
}

void EntityBase::resolveCollisions()
{
	if (!m_collisions.empty())
	{
		//Sort all collisions to be as the ones with the highest of areas are in ascending order
		std::sort(m_collisions.begin(), m_collisions.end(), sortCollisions);
		Map* map = m_entityManager->getContext().m_map;
		unsigned int tileSize = map->getTileSize();
		for (auto &i : m_collisions)
		{
			//If entity's bounding box doesn't intersect with said collision
			if (!m_AABB.intersects(i.m_tileBounds)) {
				continue;
			}
			//Calculate the difference between the two entities on the x and y axis
			float xDifference = (m_AABB.left + (m_AABB.width / 2.0f)) - (i.m_tileBounds.left + (i.m_tileBounds.width / 2.0f));
			float yDifference = (m_AABB.top + (m_AABB.height / 2.0f)) - (i.m_tileBounds.top + (i.m_tileBounds.height / 2.0f));
			float resolve = 0;
			//Collidin on x axis
			if (abs(xDifference) > abs(yDifference))
			{
				//Move entity
				if (xDifference > 0) {
					resolve = (i.m_tileBounds.left + tileSize) - m_AABB.left;
				}
				else {
					resolve = -((m_AABB.left + m_AABB.width) - i.m_tileBounds.left);
				}
				move(resolve, 0); //Move by how ever much entity was intersecting by
				m_velocity.x = 0; //Stop movement - cannot move further into entity
				m_collidingOnX = true;
			}
			//Colliding on y axis
			else
			{
				if (yDifference > 0) {
					resolve = (i.m_tileBounds.top + tileSize) - m_AABB.top;
				}
				else {
					resolve - ((m_AABB.top + m_AABB.height) - i.m_tileBounds.top);
				}
				move(0, resolve);
				m_velocity.y = 0;
				if (m_collidingOnY) {
					continue;
				}
				m_referenceTile = i.m_tile;
				m_collidingOnY = true;
			}
		}
		m_collisions.clear();
	}
	if (!m_collidingOnY) {
		m_referenceTile = nullptr;
	}
}


