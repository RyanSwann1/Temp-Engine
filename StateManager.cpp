#include "StateManager.h"
#include <iostream>


StateManager::StateManager(SharedContext& sharedContext)
{
	registerState<StateIntro>(StateType::Intro);
}


StateManager::~StateManager()
{
	purgeStates();
	purgeFactory();
}

void StateManager::switchToState(const StateType state)
{
	//If there is another state to switch to
	if (m_states.size() > 1)
	{
		//Switch to already existing state
		for (auto iter = m_states.begin(); iter != m_states.end(); iter++)
		{
			if (iter->first == state)
			{
				StateType tempStateTypeHolder = iter->first;
				StateBase* tempStateHolder = iter->second;
				//When switching to a new state, make sure that the window's view is assigned to it.
				//If not new state could be rendered outside of the windows view
				m_sharedContext.m_window->getWindow().setView(tempStateHolder->getView());
				m_states.emplace_back(tempStateTypeHolder, tempStateHolder);
				m_states.back().second->onEnter();
				m_states.erase(iter);
				iter->second->onExit();
				
				return;
			}
		}
	}

	//If no state to switch to, create instead
	createState(state);
}

void StateManager::removeState(const StateType state)
{
	//Search for state to remove
	for (auto iter = m_states.begin(); iter != m_states.end(); iter++)
	{
		if (iter->first == state)
		{
			iter->second->onDestroy();
			delete iter->second;
			iter->second = nullptr;
			m_states.erase(iter);
			return;
		}
	}
}

void StateManager::update(const sf::Time & deltaTime)
{
	for (auto &i : m_states) {
		i.second->update(deltaTime);
	}
}

void StateManager::draw(sf::RenderWindow & window)
{
	for (auto &i : m_states) 
	{
		//Assign the window to the current view of every state drawn
		m_sharedContext.m_window->getWindow().setView(i.second->m_view); 
		i.second->draw(window);
	}
}

void StateManager::processRequests()
{
	for (auto iter = m_toRemove.begin(); iter != m_toRemove.end();)
	{
		removeState(*iter);
		
		iter = m_toRemove.erase(iter);
	}
}

void StateManager::createState(const StateType state)
{
	auto iter = m_stateFactory.find(state);
	if (iter != m_stateFactory.find(state))
	{
		m_states.emplace_back(std::make_pair(iter->first, iter->second));
		m_states.back().second->onCreate();
		m_states.back().second->onEnter();
		//Assign newly created state's viewspace to that of the windows default view space
		m_states.back().second->m_view = m_sharedContext.m_window->getWindow().getDefaultView();
	}
	else {
		std::cerr << "State not found: " << "\n";
	}
}

void StateManager::purgeStates()
{
	for (auto &i : m_states) {
		delete i.second;
		i.second = nullptr;
	}
	m_states.clear();
}

void StateManager::purgeFactory()
{
	for (auto &i : m_stateFactory) {
		delete i.second();
		i.second = nullptr;
	}
	m_stateFactory.clear();
}
