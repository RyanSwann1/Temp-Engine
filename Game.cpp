#include "Game.h"

Game::Game(const std::string & windowTitle, const sf::Vector2u & windowSize)
	: m_window(windowTitle, windowSize),
	m_stateManager(m_sharedContext)
{
	m_sharedContext.m_window = &m_window;
	m_sharedContext.m_textureManager = &m_textureManager;
	m_sharedContext.m_stateManager = &m_stateManager;
}

Game::~Game()
{
}

void Game::update()
{
	m_window.update();
	m_stateManager.update(m_timeElasped);
}

void Game::render()
{
	m_window.beginDraw();
	m_stateManager.draw(m_window.getWindow());
	m_window.endDraw();
}

void Game::lateUpdate()
{
	m_timeElasped += m_clock.restart();
}
