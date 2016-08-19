#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "SharedContext.h"
#include "EntityBase.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

enum class EntityType { Base, Player, Enemy };
enum class EntityState { Idle, Walking, Jumping, Attacking, Hurt, Dying };

class EntityManager
{
public:
	EntityManager(SharedContext& sharedContext, unsigned int maxEntities);
	~EntityManager();

	int addEntity(const EntityType type, const std::string& name);
	void purgeEntities();
	void remove(unsigned int id) { m_entitiesToRemove.push_back(id); }
	
	void update(const float deltaTime);
	void draw();

	EntityBase* find(const std::string& name);
	EntityBase* find(unsigned int id);
	SharedContext& getContext() { return m_sharedContext; }

private:
	template <class T>
	void registerEntity(const EntityType type)
	{
		m_entityFactory[type] = [this]() -> EntityBase*
		{
			return new T;
		};
	}

	void processRemovals();
	void loadEnemyTypes(const std::string& name);
	void entityCollisionCheck();

	//Entity's are identified with an unsigned int. 
	std::unordered_map<unsigned int, EntityBase*> m_entityContainer;
	//Stores all possible entities available to use
	//Factory allows us to easily allocate dynamic memory of a requested class
	std::unordered_map<EntityType, std::function<EntityBase*()>> m_entityFactory;
	std::unordered_map<std::string, std::string> m_enemyTypes; //Contains name and location stored
	std::vector<unsigned int> m_entitiesToRemove;
	unsigned int m_maxEntities;
	unsigned int m_idCounter; //Keeps track of entities / how many
	SharedContext m_sharedContext;
}; 
#endif // !ENTITY_MANAGER_H

