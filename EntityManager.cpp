#include "EntityManager.h"
#include "Utilities.h"
#include <SFML\Window\Window.hpp>
#include <iostream>
#include <sstream>

EntityManager::EntityManager(SharedContext & sharedContext, unsigned int maxEntities)
	:m_sharedContext(sharedContext),
	m_maxEntities(maxEntities),
	m_idCounter(0)
{
	loadEnemyTypes("EnemyList.list");
	//Register all entites for the game
	//registerEntity<Player>(EntityType::Player);
	//registerEntity<Enemy>(EntityType::Enemy);
}

EntityManager::~EntityManager()
{
	purgeEntities();
}

void EntityManager::purgeEntities()
{
	//Delete all active entities
	for (auto &i : m_entityContainer)
	{
		delete i.second;
		i.second = nullptr;
	}
	m_entityContainer.clear();
	m_idCounter = 0; //Reset id counter
}

void EntityManager::update(const float deltaTime)
{
	for (auto &i : m_entityContainer)
	{
		i.second->update(deltaTime); //Update entity
		entityCollisionCheck(); //Check collisions
		processRemovals(); //Process the requested removed entitys
	}
}

void EntityManager::draw()
{
	sf::RenderWindow* window = &m_sharedContext.m_window->getWindow();
	sf::FloatRect viewSpace = m_sharedContext.m_window->getViewSpace();

	//If entity intersects with the game windows view space, render
	//Instead of rendering all entitys taht could potentially be out of view - thus wasting resources
	for (auto &i : m_entityContainer)
	{
		//if entity is within the viewspace of the window, draw 
		if (viewSpace.intersects(i.second->m_AABB)) {
			i.second->draw(*window);
		}
	}
}

int EntityManager::addEntity(const EntityType type, const std::string & name)
{
	auto iter = m_entityFactory.find(type);
	if (iter == m_entityFactory.end()) {
		return -1;
	}
	//Create new entity
	EntityBase* entity = iter->second();
	entity->m_id = m_idCounter;
	//Assign the entities name
	if (name != "") {
		entity->m_name = name;
	}
	m_entityContainer.emplace(entity->m_id, entity); //Add individual entity to the container
	if (type == EntityType::Enemy)
	{
		auto itr = m_enemyTypes.find(name);
		if (itr != m_enemyTypes.end())
		{
			//Enemy* enemy = (*Enemy)entity;
			//enemy->load(itr->second);
		}
	}

	m_idCounter++;
	//Entity ID that was just used gets returned to signify success
	return m_idCounter - 1; 
}

EntityBase * EntityManager::find(const std::string & name)
{
	//Find and return the requested entity
	for (auto &i : m_entityContainer)
	{
		if (i.second->getName() == name) {
			return i.second;
		}
	}
	return nullptr;
}

EntityBase * EntityManager::find(unsigned int id)
{
	//Find and return the requested entity
	auto iter = m_entityContainer.find(id);
	if (iter != m_entityContainer.end()) {
		return iter->second;
	}
	return nullptr;
}

void EntityManager::processRemovals()
{
	//Remove all entities that have been requested
	while (m_entitiesToRemove.cbegin() != m_entitiesToRemove.cend())
	{
		unsigned int id = m_entitiesToRemove.back();
		auto iter = m_entityContainer.find(id);
		if (iter != m_entityContainer.end())
		{
			//Remove requested entity
			delete iter->second;
			iter->second = nullptr;
			m_entityContainer.erase(iter);
		}
		m_entitiesToRemove.pop_back();
	}
}

void EntityManager::loadEnemyTypes(const std::string & name)
{
	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + std::string("media/Characters") + name);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << name << "\n";
		return;
	}
	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '|') { continue; }
		std::stringstream keyStream(line);
		std::string name;
		std::string charFile;
		keyStream >> name >> charFile;
		m_enemyTypes.emplace(name, charFile);
	}
	file.close();
}

void EntityManager::entityCollisionCheck()
{
	if (m_entityContainer.empty()) {
		return;
	}
	for (auto iter = m_entityContainer.begin(); std::next(iter) != m_entityContainer.end(); iter++)
	{
		for (auto iter2 = std::next(iter); iter2 != m_entityContainer.end(); iter2++)
		{
			//Do not allow to check collision on the same entity
			if (iter->first == iter2->first) {
				continue;
			}
			//Regular bounding box collision
			//Test for Movement Collision
			if (iter->second->m_AABB.intersects(iter2->second->m_AABB))
			{
				//Inform both entities they have collided with one another
				iter->second->onEntityCollision(iter2->second, false);
				iter2->second->onEntityCollision(iter->second, false);
			}

			EntityType entityType1 = iter->second->getType();
			EntityType entityType2 = iter2->second->getType();
			//Test for attack collision
			if (entityType1 == EntityType::Enemy || entityType1 == EntityType::Player)
			{
				//Character* character1 = (Character*)iter->second;
				//if(character1->m_attackAABB.intersects(iter2->second->m_AABB) {
					//character1->onEntityCollision(iter2->second, true);
				//}
			}

			//Test for attack collision
			if (entityType2 == EntityType::Enemy || entityType2 == EntityType::Player)
			{
				//Character* character2 = (Character*)iter2->second;
				//if(character2->m_attackAABB.intersects(iter->second->m_AABB)) {
					//character2->onEntityCollision(iter->second, true);
				//}
			}
		}
	}
}