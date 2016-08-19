#include "StateGame.h"
#include "StateManager.h"


StateGame::StateGame(StateManager* stateManager)
	: StateBase(stateManager),
	m_map(stateManager->getSharedContext(), this)
{
}


StateGame::~StateGame()
{
}

void StateGame::onCreate()
{
}

void StateGame::onDestroy()
{
}

void StateGame::onEnter()
{
}

void StateGame::onExit()
{
}

void StateGame::update(const sf::Time & deltaTime)
{
}

void StateGame::draw(sf::RenderWindow & window)
{
}
