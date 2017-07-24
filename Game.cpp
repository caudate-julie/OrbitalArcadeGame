#include "Game.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include <queue>

#include "Configuration.h"
#include "auxiliary.h"
#include "Flyer.h"
#include "BotFlyer.h"
#include "QueuedForGraphics.h"
#include "Logger.h"

extern Configuration* config;
extern Logger* logger;

Game* game = nullptr;

/**------------------------------------------------------------
  Creates new game with a flyer in the middle and several
  stars in the field ~ 3 times bigger than screen.
  -----------------------------------------------------------*/
Game::Game()
	: game_over(player_crashed)
	, stop_threads(false)

{
	//reset();
}

/**------------------------------------------------------------
  Destructor =)
  -----------------------------------------------------------*/
Game::~Game(void) 
{
	stop_threads = true;
	for (int i = 0; i < bot_threads.size(); i++) bot_threads[i].join();
	delete mainflyer;
	for (int i = 0; i < config->BOT_NUMBER; i++) if (bots[i]) delete bots[i];
}

void Game::reset()
{
	stars.resize(config->STAR_NUMBER);
	for (int i = 0; i < config->STAR_NUMBER; i++) 
	{ 
		change_star(i, true); 
	}

	stop_threads = true;
	for (int i = 0; i < bot_threads.size(); i++) bot_threads[i].join();
	stop_threads = false;       //  <-- FRIEND PREDICTION

	//mainflyer = std::move(unique_ptr<Flyer>(config->PLAYER_IS_BOT ? new BotFlyer() : new Flyer()));
	mainflyer = config->PLAYER_IS_BOT ? new BotFlyer() : new Flyer();   // <-- UNIQUES TO POINTERS
	dist = 0;
	player_crashed = false;

	bots.resize(config->BOT_NUMBER + (config->PLAYER_IS_BOT ? 1 : 0));
	for (int i = 0; i < config->BOT_NUMBER; i++)
	{ 
		change_bot(i); 
	}
//	if (config->PLAYER_IS_BOT) { bots[bots.size() - 1] = (static_cast<unique_ptr<BotFlyer>>(mainflyer)); }
	if (config->PLAYER_IS_BOT) { bots[bots.size() - 1] = (static_cast<BotFlyer*>(mainflyer)); }
	bot_threads.resize(config->BOT_NUMBER + (config->PLAYER_IS_BOT ? 1 : 0));
}

/**------------------------------------------------------------
  Getters for private fields.
  -----------------------------------------------------------*/
GalaxyObject  Game::player() const     { return mainflyer->info(); }
int Game::n_stars() const              { return static_cast<int>(stars.size()); }
int Game::n_bots() const               { return static_cast<int>(config->BOT_NUMBER); }
GalaxyObject  Game::star(int i) const  { return stars[i].info(); }
GalaxyObject  Game::bot(int i) const   { return bots[i]->info(); }
int Game::distance() const             { return static_cast<int>(dist / config->OUTPUT_DIST_COEFF); }

/**------------------------------------------------------------
  Starts prediction threads for all bots.
  -----------------------------------------------------------*/
void Game::start()
{
	for(int i = 0; i < bot_threads.size(); i++)
	{
		bot_threads[i].swap(std::thread(predict, i));
	}
	//  <-- FRIEND PREDICTION
	/*if (config->PLAYER_IS_BOT) { (static_cast<BotFlyer*>(mainflyer.get()))-> start(); }
	for (int i = 0; i < bots.size(); i++) 
	{
		bots[i]->start();
	}*/
}

/**------------------------------------------------------------
  Moves flyers to the next position.
  Should! also destroy crashed flyers.
  -----------------------------------------------------------*/
void Game::make_move()
{
	// TODO: add crash animation
	mainflyer->move(summ_acceleration(mainflyer->position));
	dist += (mainflyer->velocity.module());
	if (crashed(mainflyer->position, config->FLYER_SIZE)) { player_crashed = true; }

	for (int i = 0; i < bots.size(); i++) 
	{
		bots[i]->move(summ_acceleration(bots[i]->position));
		bool smb_crashed = bots[i]->im_crashed;
		if (bots[i]->im_crashed) { change_bot(i); }
	}
}

/**------------------------------------------------------------
  Asks all bots to act (decide whether they use engine).
  -----------------------------------------------------------*/
void Game::call_bots_action()
{
	// <-- UNIQUES TO POINTERS
	//if (config->PLAYER_IS_BOT) { (static_cast<BotFlyer*>(mainflyer.get()))->action(); }
	if (config->PLAYER_IS_BOT) { static_cast<BotFlyer*>(mainflyer)->action(); }
	for (int i = 0; i < bots.size(); i++) { bots[i]->action(); }
}

/**------------------------------------------------------------
  Adds engine (user-induced) acceleration to main flyer.
  Used by keyboard-processing function. Bots add up engines
  by themselves.
  -----------------------------------------------------------*/
void Game::user_turn_on_engine(char direction)
{
	mainflyer->velocity += mainflyer->engine_acceleration(direction);
}

/**------------------------------------------------------------
  Destroys every star and every bot that went ouf of scope
  (scopes differ for oblects). Creates a new object at the
  edge of scope instead of destroyed.
  -----------------------------------------------------------*/
bool Game::revise_stars()
{
	bool changed = false;
	for (int i = 0; i < stars.size(); i++)
	{
		if ((stars[i].position - mainflyer->position).module() > config->STAR_SCOPE)
		{
			change_star(i, false);
			changed = true;
		}
	}
	for (int i = 0; i < bots.size(); i++)
	{
		if ((bots[i]->position - mainflyer->position).module() > config->BOT_SCOPE)
		{
			//logger->start("bot changed");
			change_bot(i);
			//logger->stop("bot changed");
			//bots[i]->start();							//  <-- FRIEND PREDICTION
			//logger->logif("New bot thread started");	//  <-- FRIEND PREDICTION
		}
	}
	return changed;
}

/**------------------------------------------------------------
  Checks if the given point (+- size) lies inside a star - 
  for flyer means that it is crashed.
  -----------------------------------------------------------*/
bool Game::crashed(const Point& coord, double min_distance) const
{
	for (int i = 0; i < stars.size(); i++)
	{
		if ((stars[i].position - coord).module() < (stars[i].size + min_distance)) 
		{ 
			return true; 
		}
	}
	return false;
}

/**------------------------------------------------------------
  Calculates the acceleration towards given star.
  -----------------------------------------------------------*/
Point Game::acceleration(const Point& flyer_coord, const Point& star_coord, double mass) const
{
	Point r = flyer_coord - star_coord;
	return r * (- mass / pow(r.module(), 3) * config->G_CONST);
}

/**------------------------------------------------------------
  Calculates and returns summ of accelerations towards all 
  the stars in the scope.
  -----------------------------------------------------------*/
Point Game::summ_acceleration(const Point& flyer_coord) const
{
	Point a;
	for (Star s : stars)
	{
		a += acceleration(flyer_coord, s.position, s.size);
	}
	return a;
}

/**============================================================
  ===================== PRIVATE MEMBERS =======================
  ===========================================================*/

/**------------------------------------------------------------
  Adds a new star in the index position.
  * initial sets the mode of adding star: initial star is added
  anywhere in the scope,
  * not initial (in-game) must be at the edge of the scope and
  only towards players' direction.
  -----------------------------------------------------------*/
void Game::change_star(int index, bool initial)
{
	while (true) 
	{
		Point direction(d_random(0., M_PI * 2));   // shows direction from player to new star.

		// if we need a star towards flyer velocity, we may need another angle.
		if (!initial && !in_sight_semisphere(direction)) continue;
		double radius = (initial ? sqrt(d_random(0., 1.)) : d_random(0.8, 1.)) * config->STAR_SCOPE;

		Star s;
		// if star is initial, flyer is not yet created. That's needed for proper destruction order.
		s.position = direction * radius + (initial ? Point() : mainflyer->position);
		if (!crashed(s.position, s.size + config->STAR_MIN_SPACE))
		{ 
			stars[index] = s;
			return;
		}
	}
}

/**------------------------------------------------------------
  Adds a new bot to given index.
  Bots don't have "initial" mode - they always appear at the
  edge of scope.
  -----------------------------------------------------------*/
void Game::change_bot(int index)
{
	BotFlyer* f = nullptr;                          // <-- UNIQUES TO POINTERS
	while (true) 
	{
		//unique_ptr<BotFlyer> f (new BotFlyer());  // <-- UNIQUES TO POINTERS
		delete f;							// <-- UNIQUES TO POINTERS
		f = new BotFlyer();							// <-- UNIQUES TO POINTERS
		
		double angle = d_random(0., M_PI * 2);
		f->position = Point(angle) * config->BOT_SCOPE + mainflyer->position;
		f->velocity = -Point(angle) * config->INIT_VELOCITY;
		if (!crashed(f->position, config->FLYER_SIZE))
		{
			bots[index] = f; //std::move(f);		// <-- UNIQUES TO POINTERS
			return;
		}
	}
}

/**------------------------------------------------------------
  For directional star addition: checks that vector from
  flyer to star makes sharp angle with flyer's velocity.
  -----------------------------------------------------------*/
bool Game::in_sight_semisphere(const Point& direction) const
{
	return (mainflyer->velocity.x * direction.x 
		  + mainflyer->velocity.y * direction.y) > 0;
}


/**------------------------------------------------------------
  This is a function to predict flight, running into separate
  thread. The result goes into this->recommendation.
  Also it checks for already-crashed to relieve main thread.
  -----------------------------------------------------------*/
void predict(int i)				//  <-- FRIEND PREDICTION
{
	 while (!game->stop_threads) 
	{
		//logger->start("bot prediction");
		if (game->crashed(game->bots[i]->position, config->FLYER_SIZE)) 
		{ 
			game->bots[i]->im_crashed = true; 
		}

		Point position = game->bots[i]->position;
		Point velocity = game->bots[i]->velocity;

		//logger->start("flat flight");
		int flat_crash = game->bots[i]->mock_flight(position, game->bots[i]->velocity);
		if (flat_crash >= config->BOT_MAX_STEPS) 
		{ 
			game->bots[i]->recommendation = 'N';
			continue; 
		}
		//logger->stop("flat flight");
		// no crash happened - no interaction needed.
	
		//logger->start("acceleration");
		Point left_turn = game->bots[i]->engine_acceleration('L');
		Point right_turn = game->bots[i]->engine_acceleration('R');
		//logger->stop("acceleration");

		//logger->start("turn_flight");
		int left_crash = game->bots[i]->mock_flight(position, velocity + left_turn);
		int right_crash = game->bots[i]->mock_flight(position, velocity + right_turn);
		//logger->stop("turn_flight");

		if ((left_crash <= flat_crash) && (right_crash <= flat_crash)) 
		{
			game->bots[i]->recommendation = 'N';
			continue;  // flat flight is longest.
		}
		game->bots[i]->recommendation = (left_crash > right_crash) ? 'L' : 'R';
		//logger->stop("bot prediction");
	}
}