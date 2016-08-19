#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <SFML\Graphics.hpp>
#include <SFML\Window.hpp>
#include <vector>
#include <unordered_map>
#include <functional>

enum class EventType {
	KeyDown = sf::Event::KeyPressed,
	KeyUp = sf::Event::KeyReleased,
	MButtonDown = sf::Event::MouseButtonPressed,
	MButtonUp = sf::Event::MouseButtonReleased,
	MouseWheel = sf::Event::MouseWheelMoved,
	WindowResized = sf::Event::Resized,
	GainedFocus = sf::Event::GainedFocus,
	LostFocus = sf::Event::LostFocus,
	MouseEntered = sf::Event::MouseEntered,
	MouseLeft = sf::Event::MouseLeft,
	Closed = sf::Event::Closed,
	TextEntered = sf::Event::TextEntered,
	Keyboard = sf::Event::Count + 1, Mouse, Joystick
};

struct EventInfo
{
	EventInfo()
		:m_code(0) {}
	EventInfo(int i)
		:m_code(i) {}
	union 
	{
		int m_code; //The way to see what the code of the event is 
	};
};

struct EventDetails
{
	EventDetails(const std::string& bindName) {}

	std::string m_bindName;
	sf::Vector2i m_size;
	sf::Uint32 m_textEntered;
	sf::Vector2i m_mouse;
	int m_mouseWheelData;
	int m_keyCode; //Single key code

	void clear()
	{
		m_size = sf::Vector2i(0, 0);
		m_textEntered = 0;
		m_mouse = sf::Vector2i(0, 0);
		m_mouseWheelData = 0;
		m_keyCode = -1;
	}
};

struct Binding
{
	Binding(const std::string& bindName)
		:m_bindName(bindName),
		m_eventDetails(bindName),
		m_eventCount(0) {}

	//Binds a new event		
	//Uses the default arguments 'EventInfo' default constructor 
	//so thjat atleast the member variable is set to zero
	void bindEvent(const EventType type, EventInfo eventInfo = EventInfo()) { m_events.emplace_back(type, eventInfo); }

	std::vector<std::pair<EventType, EventInfo>> m_events;
	std::string m_bindName;
	int m_eventCount; //Count of events that are currently happening
	EventDetails m_eventDetails;
};

using Callbacks = std::unordered_map<std::string, std::function<void(EventDetails*)>>;

class EventManager
{
public:
	EventManager();
	~EventManager();

	bool addBinding(const Binding& binding);
	bool removeBinding(const std::string& bindingName);
	void update(sf::Event& event);
	void handleEvent(sf::Event& event);

private:
	std::unordered_map<std::string, Binding*> m_bindings;
	Callbacks m_callBacks;
	bool m_hasFocus;
	
	void loadBindings();
};

#endif // !EVENT_MANAGER_H