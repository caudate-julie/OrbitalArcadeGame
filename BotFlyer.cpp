#include "BotFlyer.h"
#include "Game.h"
#include "GameGraphics.h"
#include <cassert>


/**------------------------------------------------------------
  Number of calculated steps of flat flights.
  -----------------------------------------------------------*/
const int BotFlyer::MAX_STEPS = 300;

/**------------------------------------------------------------
  Creates a new bot. Count is for debug output.
  -----------------------------------------------------------*/
BotFlyer::BotFlyer(void) : prediction() 
{
	this->stop_thread = false;
	this->turn = 'N';

}

/**------------------------------------------------------------
  Makes class with info about the object to pass outside
  Game class (mainly to graphics class).
  -----------------------------------------------------------*/
BotFlyer::BotFlyer(const BotFlyer& other) : prediction() { *this = other; }

/**------------------------------------------------------------
  This method is called when the bot is finally constructed,
  placed and flying and can start predicting its way.
  -----------------------------------------------------------*/
void BotFlyer::start()
{
	this->prediction.swap(std::thread(&BotFlyer::predict, this));

}

/**------------------------------------------------------------
  Stops prediction thread.
  -----------------------------------------------------------*/
BotFlyer::~BotFlyer(void) 
{
	this->stop_thread = true;
	if (this->prediction.joinable()) this->prediction.join();
}

/**------------------------------------------------------------
  Bot decides whether it should turn on engine.
  It counts three MAX_STEP-length paths : flat flight (1),
  turn left (2) or right (3) in the beginning, and than chooses
  the one with longest time before crash.
  -----------------------------------------------------------*/
void BotFlyer::action()
{
	if (this->turn != 'N')
	{
		char t;
		//while (!this->mutex_on_turn.try_lock()) {}
		std::lock_guard<std::mutex> lock(mutex_on_turn);
		t = this->turn;
		this->turn = 'N';
		//this->mutex_on_turn.unlock();
		this->velocity += this->engine_acceleration(t);
	}

}

GalaxyObject BotFlyer::info() const 
{
	GalaxyObject my_info = Flyer::info();
	my_info.subtype = bot;
	return my_info;
}

BotFlyer& BotFlyer::operator=(const BotFlyer& other)
{
	if (this != &other)
	{
		// stop_thread and turn are not copied.
		this->position = other.position;
		this->velocity = other.velocity;
	}
	return *this;
}

void BotFlyer::predict()
{
	while (!this->stop_thread) {
		int flat_crash = this->mock_flight(this->position, this->velocity);
		if (flat_crash == MAX_STEPS + 1) { continue; }      // no crash happened - no interaction needed.
	
		int left_crash = this->mock_flight(this->position, this->velocity + this->engine_acceleration('L'));
		int right_crash = this->mock_flight(this->position, this->velocity + this->engine_acceleration('R'));

		if ((left_crash < flat_crash) && (right_crash < flat_crash)) continue; // flat flight is longest.
		this->turn = (left_crash > right_crash) ? 'L' : 'R';
	}
}

/**------------------------------------------------------------
  Flat flight from given point with given velocity, MAX_STEP
  steps. Returns the step where crash happened (if no, returns
  MAX_STEP + 1). No star change taken into account.
  -----------------------------------------------------------*/
int BotFlyer::mock_flight(Point p, Point v) const
{
	int step = 0;
	Game& game = Game::get();
	while (step < MAX_STEPS)
	{
		v += game.summ_acceleration(p);
		p += v;
		if (game.crashed(p, Configuration::get().FLYER_SIZE)) { return step; }
		step++;
	}
	return MAX_STEPS + 1;
}
