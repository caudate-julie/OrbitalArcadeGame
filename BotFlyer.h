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

	bool im_crashed;
	//bool stop_thread;   //  <-- FRIEND PREDICTION

	//void start();       //  <-- FRIEND PREDICTION
	void action();

	BotFlyer& operator=(const BotFlyer& other);

private:
	char recommendation;
	//std::thread prediction;  //  <-- FRIEND PREDICTION

	//void predict();       //  <-- FRIEND PREDICTION
	friend void predict(int i);
	int mock_flight(Point p, Point v) const;
};
