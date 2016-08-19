#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include "StateBase.h"
#include "StateIntro.h"
#include "SharedContext.h"
#include <unordered_map>
#include <vector>
#include <functional>

enum class StateType{ Intro, MainMenu, Game, Credits };
class StateManager
{
public:
	StateManager(SharedContext& sharedContext);
	~StateManager();

	void switchToState(const StateType state);
	void removeState(const StateType state);
	void remove(const StateType state) { m_toRemove.push_back(state); }
	void update(const sf::Time& deltaTime);
	void draw(sf::RenderWindow& window);

	void processRequests();

	SharedContext& getSharedContext() { return m_sharedContext; }

private:
	std::unordered_map<StateType, std::function<StateBase*()>> m_stateFactory; //Get states from here
	std::vector<std::pair<StateType, StateBase*>> m_states; //States currently being used
	std::vector<StateType> m_toRemove;
	SharedContext m_sharedContext;

	void createState(const StateType state);
	void purgeStates();
	void purgeFactory();

	template <class T>
	void registerState(const StateType state)
	{
		m_stateFactory[state] = [this]() -> StateBase*
		{
			return new T;
		}
	}
};
#endif // !STATE_MANAGER_H
