#ifndef SHARED_CONTEXT_H
#define SHARED_CONTEXT_H
#include "Windoww.h"
#include "TextureManager.h"

class EntityManager;
class StateManager;
class Map;
struct SharedContext
{
	SharedContext()
		: m_window(nullptr),
		m_textureManager(nullptr),
		m_stateManager(nullptr),
		m_map(nullptr)
	{

	}

	Windoww* m_window;
	TextureManager* m_textureManager;
	StateManager* m_stateManager;
	EntityManager* m_entityManager;
	Map* m_map;
};
#endif // !SHARED_CONTEXT_H

