#pragma once

#include <thread>
#include <mutex>

#include "flyer.h"

class BotFlyer :
	public Flyer
{
public:
	BotFlyer(void);
	~BotFlyer(void) override;
	virtual GalaxyObject info() const;

	bool crashed;

	void start();
	void action();

	BotFlyer& operator=(const BotFlyer& other);

private:
	std::thread prediction;
	bool stop_thread;
	char recommendation;

	void predict();
	int mock_flight(Point p, Point v) const;
};
