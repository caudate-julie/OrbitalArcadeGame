#pragma once
#include <SFML\Graphics.hpp>

class TimedEvent
{
public:
	TimedEvent(int interval);
	virtual void on_time(int current_time);

private:
	int interval;
	int trigger_time;
	virtual void execute() = 0;
};

/*-------------------------------------------------------------*/
class TimedGameMove : public TimedEvent
{
public:
	TimedGameMove();

private:
	void execute() override;
};

/*-------------------------------------------------------------*/
class TimedStarRevision : public TimedEvent
{
public:
	TimedStarRevision();

private:
	void execute() override;
};

/*-------------------------------------------------------------*/
class TimedBotAction : public TimedEvent
{
public:
	TimedBotAction();

private:
	void execute() override;
};

