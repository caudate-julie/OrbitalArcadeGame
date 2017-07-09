#include "BotFlyer.h"

#include <cassert>

#include "Game.h"
#include "GameGraphics.h"
#include "Configuration.h"

extern Configuration* config;
extern Game* game;
extern GameGraphics* gamegraphics;

/**------------------------------------------------------------
  Creates a new bot. Count is for debug output.
  -----------------------------------------------------------*/
BotFlyer::BotFlyer(void) 
	: im_crashed(false)
	, recommendation('N')
//	, prediction()             //  <-- FRIEND PREDICTION
//	, stop_thread(false)       //  <-- FRIEND PREDICTION
{}

/**------------------------------------------------------------
  Void destructor.
  -----------------------------------------------------------*/
BotFlyer::~BotFlyer(void) {
	//this->stop_thread = true;      //  <-- FRIEND PREDICTION
	//if (this->prediction.joinable()) this->prediction.join();  //  <-- FRIEND PREDICTION
}

/**------------------------------------------------------------
  This method is called when the bot is finally constructed,
  game is finally constructed with star list, and bot can
  start predicting its flight.
  -----------------------------------------------------------*/
/*void BotFlyer::start()			//  <-- FRIEND PREDICTION
{
	prediction.swap(std::thread(&BotFlyer::predict, this));
}*/

/**------------------------------------------------------------
  Bot decides whether it should turn on engine.
  It looks at the *last recommendation given by the thread and
  implements it.
  -----------------------------------------------------------*/
void BotFlyer::action()
{
	if (recommendation != 'N')
	{
		char turn;
//		std::lock_guard<std::mutex> recommendationGuard(mutex_on_turn);
		turn = recommendation;
		recommendation = 'N';
		velocity += engine_acceleration(turn);
	}
}

/**------------------------------------------------------------
  Duplicates parent's method but changes subtype to bot.
  -----------------------------------------------------------*/
GalaxyObject BotFlyer::info() const 
{
	GalaxyObject my_info = Flyer::info();
	my_info.subtype = bot;
	return my_info;
}

/**------------------------------------------------------------
  Copies the bot. Stop_thread and turn are not copied,
  new bot should call .start method to start a thread.
  -----------------------------------------------------------*/
BotFlyer& BotFlyer::operator=(const BotFlyer& other)
{
	if (this != &other)
	{
		position = other.position;
		velocity = other.velocity;
		im_crashed = other.im_crashed;
	}
	return *this;
}

/**============================================================
  ===================== PRIVATE MEMBERS =======================
  ===========================================================*/

/**------------------------------------------------------------
  Flat flight from given point with given velocity, MAX_STEP
  steps. Returns the step where crash happened (if no, returns
  MAX_STEP + 1). No possible star change taken into account.
  -----------------------------------------------------------*/
int BotFlyer::mock_flight(Point p, Point v) const
{
	int step = 0;
	while (step < config->BOT_MAX_STEPS)
	{
		v += game->summ_acceleration(p);
		p += v;
		if (game->crashed(p, config->FLYER_SIZE)) { break; }
		step++;
	}
	return step + 1;
}

/**------------------------------------------------------------
  This is a function to predict flight, running into separate
  thread. The result goes into this->recommendation.
  Also it checks for already-crashed to relieve main thread.
  -----------------------------------------------------------*/
/*void BotFlyer::predict()				//  <-- FRIEND PREDICTION
{
	sf::Clock clock;
	 while (!this->stop_thread) 
	{
		clock.restart();
		if (game->crashed(position, config->FLYER_SIZE)) { im_crashed = true; }

		int flat_crash = mock_flight(position, velocity);
		if (flat_crash >= config->BOT_MAX_STEPS) 
		{ 
			recommendation = 'N';
			continue; 
		}
		// no crash happened - no interaction needed.
	
		int left_crash = mock_flight(position, velocity + engine_acceleration('L'));
		int right_crash = mock_flight(position, velocity + engine_acceleration('R'));

		if ((left_crash <= flat_crash) && (right_crash <= flat_crash)) 
		{
			recommendation = 'N';
			continue;  // flat flight is longest.
		}
		int time = clock.getElapsedTime().asMicroseconds();
		recommendation = (left_crash > right_crash) ? 'L' : 'R';
	}
	int k = 5;
}*/

