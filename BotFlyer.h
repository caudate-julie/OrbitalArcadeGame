#pragma once
#include "flyer.h"
#include <thread>
#include <mutex>

class BotFlyer :
	public Flyer
{
public:
	BotFlyer(void);
	BotFlyer(const BotFlyer&);
	~BotFlyer(void);
	void start();
	void action();
	virtual GalaxyObject info() const;

	BotFlyer& operator=(const BotFlyer& other);

private:
	std::thread prediction;
	std::mutex mutex_on_turn;
	bool stop_thread;
	char turn;

	void predict();
	int mock_flight(Point p, Point v) const;
};
