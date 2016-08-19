#include "EventManager.h"



EventManager::EventManager()
	: m_hasFocus(true)
{
	loadBindings();
}


EventManager::~EventManager()
{
	//Purge all bindings
	for (auto &i : m_bindings)
	{
		delete i.second;
		i.second = nullptr;
	}
	m_bindings.clear();
}

bool EventManager::addBinding(const Binding & binding)
{
	//Don't allow binding to be added that already exists
	if (m_bindings.find(binding.m_bindName) != m_bindings.end()) {
		return false;
	}

	//Test on added to existing bindings
	return m_bindings.emplace(binding.m_bindName, binding).second;
}

bool EventManager::removeBinding(const std::string & bindingName)
{
	auto iter = m_bindings.find(bindingName);
	if (iter != m_bindings.end())
	{
		delete iter->second;
		iter->second = nullptr;
		m_bindings.erase(iter);
		return true; //Binding successfully deleted
	}
	//Binding not found
	return false;
}

void EventManager::update(sf::Event& event)
{
	if (!m_hasFocus) {
		return;
	}
	for (auto &bindIter : m_bindings)
	{
		Binding* binding = bindIter.second;
		for (auto &eventIter : binding->m_events)
		{
			switch (eventIter.first)
			{
				//Keyboard events
			case (EventType::Keyboard) :
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(eventIter.second.m_code)))
				{
					if (binding->m_eventDetails.m_keyCode != -1) {
						binding->m_eventDetails.m_keyCode = eventIter.second.m_code;
					}
					binding->m_eventCount++;

					
				}
				break;
				//Mouse event
			case (EventType::Mouse) : 
				if (sf::Mouse::isButtonPressed(sf::Mouse::Button(eventIter.second.m_code)))
				{
					if (binding->m_eventDetails.m_keyCode != -1) {
						binding->m_eventDetails.m_keyCode = eventIter.second.m_code;
					}
				}
			}
			break;
			
		}
	}
}

void EventManager::handleEvent(sf::Event & event)
{
	//See if event matches the event that is being currently processed

	//Upon each iteration have a look at given events
	//And see if ther is anything we are interested in
	for (auto &bindIter : m_bindings)
	{
		Binding* binding = bindIter.second;
		for (auto eventIter : binding->m_events)
		{
			EventType sfmlEvent = (EventType)event.type; //Event that has been sent in
			if (eventIter.first != sfmlEvent) { //If event not recognised
				continue;
			}
			//If keyboard event
			if (sfmlEvent == EventType::KeyDown || sfmlEvent == EventType::KeyUp)
			{
				//Check for matching event/keystroke
				if (eventIter.second.m_code == event.key.code)
				{
					if (binding->m_eventDetails.m_keyCode != -1) { //Cannot be mnatched if above this number - I think
						binding->m_eventDetails.m_keyCode = eventIter.second.m_code;
					}
					binding->m_eventCount++; //Increase number of con-current events
					break;
				}
			}
			else if (sfmlEvent == EventType::MButtonDown || sfmlEvent == EventType::MButtonUp)
			{
				//Matching event/keystroke
				if (eventIter.second.m_code == event.mouseButton.button)
				{
					binding->m_eventDetails.m_mouse.x = event.mouseButton.x;
					binding->m_eventDetails.m_mouse.y = event.mouseButton.y;
					if (binding->m_eventDetails.m_keyCode != -1) {
						binding->m_eventDetails.m_keyCode = eventIter.second.m_code;
					}
					binding->m_eventCount++;
					break;
				}
			}
			else
			{
				//No need for additional checking
				if (sfmlEvent == EventType::MouseWheel) {
					binding->m_eventDetails.m_mouseWheelData = event.mouseWheel.delta;
				}
				else if (sfmlEvent == EventType::WindowResized) {
					binding->m_eventDetails.m_size.x = event.size.width;
					binding->m_eventDetails.m_size.y = event.size.height;
				}
				else if (sfmlEvent == EventType::TextEntered) {
					binding->m_eventDetails.m_textEntered = event.text.unicode;
				}
				binding->m_eventCount++;
			}
		}
	}
}

void EventManager::loadBindings()
{

}
