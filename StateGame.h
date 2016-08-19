#ifndef STATE_GAME_H
#define STATE_GAME_H

#include "Map.h"
#include "StateBase.h"
class StateGame :
	public StateBase
{
public:
	StateGame(StateManager* stateManager);
	~StateGame();

	void onCreate() override;
	void onDestroy() override;
	
	void onEnter() override;
	void onExit() override;

	void update(const sf::Time& deltaTime) override;
	void draw(sf::RenderWindow& window) override;

private:
	Map m_map;
};
#endif // !STATE_GAME_H
