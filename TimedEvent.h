#pragma once
#include <SFML\Graphics.hpp>

class TimedEvent
{
public:
	TimedEvent(void);
	~TimedEvent(void);

	sf::Clock* clock;
	sf::Time trigger_time;
	virtual void execute() = 0;

};

