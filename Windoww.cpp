#include "Windoww.h"

Windoww::Windoww(const std::string & windowTitle, const sf::Vector2u & windowSize)
{
	setup(windowTitle, windowSize);
}

Windoww::~Windoww()
{
	m_window.close();
}

void Windoww::update()
{
	sf::Event m_event;
	while (m_window.pollEvent(m_event))
	{
		switch (m_event.type)
		{
		case sf::Event::Closed:
			m_window.close();
			break;

		default:
			break;
		}
	}
}

sf::FloatRect & Windoww::getViewSpace()
{
	sf::Vector2f viewCenter = m_window.getView().getCenter();
	sf::Vector2f viewSize = m_window.getView().getSize();
	sf::Vector2f halfSize = sf::Vector2f(viewSize.x / 2.0f, viewSize.y / 2.0f);
	sf::FloatRect viewSpace(viewCenter - halfSize, viewSize);
	return viewSpace;

}

void Windoww::setup(const std::string & windowTitle, const sf::Vector2u & windowSize)
{
	m_windowTitle = windowTitle;
	m_windowSize = windowSize;
	m_fullScreen = false;
	create();
}

void Windoww::create()
{
	auto style = (m_fullScreen ? sf::Style::Fullscreen : sf::Style::Default);
	m_window.create(sf::VideoMode(m_windowSize.x, m_windowSize.y, 32), m_windowTitle, style);
}
