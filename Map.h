#ifndef MAP_H
#define MAP_H
#include "SharedContext.h"
#include "StateBase.h"
#include <string>
#include <unordered_map>
#include <SFML\Graphics.hpp>

struct TileSheet 
{
	static const unsigned int TileSize = 32;
	static const unsigned int ScreenWidth = 256;
	static const unsigned int ScreenHeight = 256;
};

//Using the fly weight pattern
//Meaning that for each tile, it's texture isn't stored for that tile and is retrieved from else where
//Thus saving a great deal of potential wasted memory
struct TileInfo
{
	TileInfo(SharedContext& sharedContext, const std::string& textureName = "", unsigned int tileID = 0)
		: m_sharedContext(sharedContext),
		m_textureName(textureName),
		m_tileID(tileID),
		m_deadly(false)
	{
		TextureManager* textureManager = m_sharedContext.m_textureManager;
		if (textureManager->requireResource(m_textureName))
		{
			m_tileID = tileID;
			m_textureName = textureName;
			m_sprite.setTexture(*textureManager->getResource(m_textureName));
		}
	}

	~TileInfo()
	{
		if (m_textureName != "") {
			m_sharedContext.m_textureManager->releaseResource(m_textureName);
		}
	}

	sf::Vector2f m_friction;
	SharedContext& m_sharedContext;
	unsigned int m_tileID;
	sf::Sprite m_sprite;
	std::string m_textureName;
	bool m_deadly;
};

struct Tile
{
	TileInfo* m_tileInfo;
	bool m_warp; //Meaning to load next level - when player steps on 
};

class Map
{
public:
	Map(SharedContext& sharedContext, StateBase* currentState);
	~Map();

	sf::Vector2u getMapSize() const { return m_maxMapSize; }
	TileInfo* getDefaultTile()  { return &m_defaultTile; }
	unsigned int getTileSize() const { return TileSheet::TileSize; }
	float getGravity() const { return m_mapGravity; }
	void loadTiles(const std::string& path); //Load in every single seperate tile
	void loadMap(const std::string& path); //Load in the game map
	void loadNextMap() { m_loadNextMap = true; }

	void update(const sf::Time& deltaTime);
	void draw(sf::RenderWindow& window);
	void changeMap(const std::string& path);
	Tile* getTile(unsigned int x, unsigned int y);

private:
	std::unordered_map<unsigned int, TileInfo*> m_tileSet;
	std::unordered_map<unsigned int, Tile*> m_tileMap;

	unsigned int convertCoords(unsigned int x, unsigned int y) { return (x * m_maxMapSize.x) + x; }

	void purgeTileMap();
	void purgeTileSet();

	bool m_loadNextMap;
	sf::Sprite m_background;
	std::string m_nextMap;
	std::string m_backgroundTextureName;
	float m_mapGravity;
	TileInfo m_defaultTile;
	sf::Vector2f m_defaultFriction;
	sf::Vector2u m_maxMapSize;
	sf::Vector2f m_playerStartPos;
	SharedContext m_sharedContext;
	StateBase* m_currentState;
};
#endif // !MAP_H
