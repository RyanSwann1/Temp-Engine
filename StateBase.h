#ifndef STATE_BASE_H
#define STATE_BASE_H

#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Graphics\View.hpp>
#include <SFML\System\Time.hpp>

class StateManager;
class StateBase
{
	friend StateManager;
public:
	StateBase(StateManager* stateManager)
		: m_stateManager(stateManager) {}
	virtual ~StateBase() {}

	virtual void onCreate() {}
	virtual void onDestroy() {}

	virtual void onEnter() {}
	virtual void onExit() {}
	
	virtual void update(const sf::Time& time) = 0;
	virtual void draw(sf::RenderWindow& window) = 0;

	sf::View& getView() { return m_view; }

protected:
	StateManager* m_stateManager;
	sf::View m_view;
};
#endif // !STATE_BASE_H
