#ifndef GAME_H
#define GAME_H

#include "Windoww.h"
#include "SharedContext.h"
#include "StateManager.h"
#include "TextureManager.h"
#include <string>
#include <SFML\Graphics.hpp>
#include <SFML\System\Time.hpp>

class Game
{
public:
	Game(const std::string& windowTitle, const sf::Vector2u& windowSize);
	~Game();

	//Helper functions
	bool isRunning() const { return m_window.isOpen(); }

	void update();
	void render();
	void lateUpdate();

private:
	Windoww m_window;
	SharedContext m_sharedContext;
	StateManager m_stateManager;
	TextureManager m_textureManager;
	sf::Time m_timeElasped;
	sf::Clock m_clock;

};
#endif // !GAME_H
