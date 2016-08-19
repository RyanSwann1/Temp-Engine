#ifndef STATE_INTRO_H
#define STATE_INTRO_H


#include "StateBase.h"
class StateIntro :
	public StateBase
{
public:
	StateIntro(StateManager* const stateManager);
	~StateIntro() override;

	void update(const sf::Time& time) override;
	void draw(sf::RenderWindow& window) override;

private:

};
#endif // !STATE_INTRO_H
