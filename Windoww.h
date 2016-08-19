#ifndef WINDOW_H
#define WINDOW_H

#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>
#include <string>
class Windoww
{
public:
	Windoww(const std::string& windowTitle, const sf::Vector2u& windowSize);
	~Windoww();

	void update();
	void beginDraw() { m_window.clear(sf::Color::Black); }
	void draw(const sf::Drawable& drawable) { m_window.draw(drawable); }
	void endDraw() { m_window.display(); }

	bool isOpen() const { return m_window.isOpen(); }

	sf::RenderWindow& getWindow() { return m_window; }
	sf::FloatRect& getViewSpace(); //Get centre of the viewSpace

private:
	sf::RenderWindow m_window;
	std::string m_windowTitle;
	sf::Vector2u m_windowSize;
	bool m_fullScreen;
	
	void setup(const std::string& windowTitle, const sf::Vector2u& windowSize);
	void create();
};

#endif // !WINDOW_H