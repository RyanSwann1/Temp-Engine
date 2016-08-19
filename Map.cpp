#include "Map.h"
#include "Utilities.h"
#include "StateManager.h"
#include <fstream>
#include <sstream>
#include <math.h>


Map::Map(SharedContext& sharedContext, StateBase* currentState)
	: m_sharedContext(sharedContext),
	m_currentState(currentState),
	m_nextMap(false)
{
	m_sharedContext.m_map = this;
	loadTiles("tiles.cfg");
}


Map::~Map()
{
	purgeTileMap();
	purgeTileSet();
	m_sharedContext.m_map = nullptr;
	m_sharedContext.m_textureManager->releaseResource(m_backgroundTextureName);
}

void Map::loadTiles(const std::string& path)
{
	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + path);
	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			std::stringstream keyStream(line);
			int id;
			std::string name;
			if (line[0] == '|') {
				continue;
			}
			keyStream >> id;
			keyStream >> name;
			TileInfo* tile = new TileInfo(m_sharedContext, name, id);
			keyStream >> tile->m_friction.x >> tile->m_friction.y >> tile->m_deadly;
			if (!m_tileSet.emplace(id, tile).second)
			{
				std::cerr << "Unable to load tile: " << name << "\n";
				delete tile;
				tile = nullptr;
			}
		}
		file.close();
	}
}

void Map::loadMap(const std::string & path)
{
	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + path);
	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			std::stringstream keyStream(line);
			std::string type;
			keyStream >> type;
			if (type == "SIZE") {
				keyStream >> m_maxMapSize.x >> m_maxMapSize.y;
			}
			else if (type == "DEFAULT_FRICTION") {
				keyStream >> m_defaultFriction.x >> m_defaultFriction.y;
			}
			else if (type == "GRAVITY") {
				keyStream >> m_mapGravity;
			}
			else if (type == "NEXTMAP") {
				keyStream >> m_nextMap;
			}
			else if (type == "TILE")
			{
				int id;
				keyStream >> id;
				//Search to see if tile already exists
				auto iter = m_tileSet.find(id);
				if (iter == m_tileSet.end()) { return; }

				Tile* tile = new Tile;
				tile->m_tileInfo = iter->second;
				iter->second = tile->m_tileInfo;
				sf::Vector2i tileCoords;
				keyStream >> tileCoords.x >> tileCoords.y;
				if (!m_tileMap.emplace(convertCoords(tileCoords.x, tileCoords.y), tile).second)
				{
					delete tile;
					tile = nullptr;
					continue; //Continue onto next line
				}

				std::string warp;
				tile->m_warp = false;
				keyStream >> warp;
				if (warp == "WARP") {
					tile->m_warp = true;
				}
			}
			else if (type == "BACKGROUND")
			{
				TextureManager* textureManager = m_sharedContext.m_textureManager;
				keyStream >> m_backgroundTextureName;
				if (!textureManager->requireResource(m_backgroundTextureName))
				{
					std::cerr << "Unable to load background." << m_backgroundTextureName << "\n";
				}
				sf::Texture* texture = textureManager->getResource(m_backgroundTextureName);
				m_background.setTexture(*texture);
				//Scale the view up to the size of the current states view size
				sf::Vector2f viewSpace = m_currentState->getView().getSize();
				sf::Vector2f scaleFactor;
				scaleFactor.x = viewSpace.x / texture->getSize().x;
				scaleFactor.x = viewSpace.y / texture->getSize().y;
				m_background.setScale(scaleFactor);
			}
		}
		file.close();
	}
}

void Map::update(const sf::Time & deltaTime)
{
	//User has requested a map change
	if (m_loadNextMap)
	{
		purgeTileMap();
		m_loadNextMap = false;
		if (m_nextMap != "") { //If new map has been set
			loadMap(m_nextMap);
		}
		else {
			m_sharedContext.m_stateManager->switchToState(StateType::MainMenu);
		}
	}

	//Update background pos to keep align with the view of the game
	sf::FloatRect viewSpace = m_sharedContext.m_window->getViewSpace();
	m_background.setPosition(viewSpace.left, viewSpace.top);

}

void Map::draw(sf::RenderWindow & window)
{
	//Only draw certain parts of the map that are within the view
	sf::FloatRect viewSpace;
}

void Map::changeMap(const std::string & path)
{
	m_nextMap = true;
	m_nextMap = path;
}

Tile * Map::getTile(unsigned int x, unsigned int y)
{
	auto iter = m_tileMap.find(convertCoords(x, y));
	return (iter != m_tileMap.end() ? iter->second : nullptr);
}


void Map::purgeTileMap()
{
	for (auto &i : m_tileMap) {
		delete i.second;
		i.second = nullptr;
	}
	m_tileMap.clear();
}

void Map::purgeTileSet()
{
	for (auto &i : m_tileSet)
	{
		delete i.second;
		i.second = nullptr;
	}
	m_tileSet.clear();
}
