#include "BotFlyer.h"
#include "Game.h"
#include "GameGraphics.h"
#include <cassert>

extern Configuration* config;
extern Game* game;

/**------------------------------------------------------------
  Creates a new bot. Count is for debug output.
  -----------------------------------------------------------*/
BotFlyer::BotFlyer(void) 
	: prediction() 
{
	this->stop_thread = false;
	recommendation = 'N';
}

/**------------------------------------------------------------
  Void destructor.
  -----------------------------------------------------------*/
BotFlyer::~BotFlyer(void) {
	this->stop_thread = true;
	if (this->prediction.joinable()) this->prediction.join();
}

/**------------------------------------------------------------
  This method is called when the bot is finally constructed,
  game is finally constructed with star list, and bot can
  start predicting its flight.
  -----------------------------------------------------------*/
void BotFlyer::start()
{
	prediction.swap(std::thread(&BotFlyer::predict, this));
}

/**------------------------------------------------------------
  Bot decides whether it should turn on engine.
  It looks at the last recommendation given by the thread and
  implements it.
  -----------------------------------------------------------*/
void BotFlyer::action()
{
	if (recommendation != 'N')
	{
		char turn;
		{
			// we don't double-check turn here, so in case it happened to change
			// engine_acceleration can also handle 'N'-symbol.
			std::lock_guard<std::mutex> recommendationGuard(mutex_on_turn);
			turn = recommendation;
			recommendation = 'N';
		}
		// engine_acceleration is a long method, so is held out of lock scope.
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
	}
	return *this;
}

/**============================================================
  ===================== PRIVATE MEMBERS =======================
  ===========================================================*/

/**------------------------------------------------------------
  This is a function to predict flight, running into separate
  thread. The result goes into this->recommendation.
  -----------------------------------------------------------*/
void BotFlyer::predict()
{
	while (!this->stop_thread) {
		int flat_crash = mock_flight(position, velocity);
		if (flat_crash == (config->BOT_MAX_STEPS + 1)) { continue; }
		// no crash happened - no interaction needed.
	
		int left_crash = mock_flight(position, velocity + engine_acceleration('L'));
		int right_crash = mock_flight(position, velocity + engine_acceleration('R'));

		if ((left_crash < flat_crash) && (right_crash < flat_crash)) continue;  // flat flight is longest.
		recommendation = (left_crash > right_crash) ? 'L' : 'R';
	}
}

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
	}
	return step + 1;
}
