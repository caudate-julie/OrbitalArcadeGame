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

extern Configuration* config;

/**------------------------------------------------------------
  Creates new game with a flyer in the middle and several
  stars in the field ~ 3 times bigger than screen.
  -----------------------------------------------------------*/
Game::Game()
	: game_over(player_crashed)
{
	//reset();
}

/**------------------------------------------------------------
  Destructor =)
  -----------------------------------------------------------*/
Game::~Game(void) { }

void Game::reset()
{
	stars.resize(config->STAR_NUMBER);
	for (int i = 0; i < config->STAR_NUMBER; i++) 
	{ 
		change_star(i, true); 
	}

	mainflyer = std::move(unique_ptr<Flyer>(config->PLAYER_IS_BOT ? new BotFlyer() : new Flyer()));
	dist = 0;
	player_crashed = false;

	bots.resize(config->BOT_NUMBER);
	for (int i = 0; i < config->BOT_NUMBER; i++)
	{ 
		change_bot(i); 
	}
}

/**------------------------------------------------------------
  Getters for private fields.
  -----------------------------------------------------------*/
GalaxyObject  Game::player() const     { return mainflyer->info(); }
int Game::n_stars() const              { return static_cast<int>(stars.size()); }
int Game::n_bots() const               { return static_cast<int>(bots.size()); }
GalaxyObject  Game::star(int i) const  { return stars[i].info(); }
GalaxyObject  Game::bot(int i) const   { return bots[i]->info(); }
int Game::distance() const             { return static_cast<int>(dist / config->OUTPUT_DIST_COEFF); }

/**------------------------------------------------------------
  Starts prediction threads for all bots.
  -----------------------------------------------------------*/
void Game::start()
{
	if (config->PLAYER_IS_BOT) { (static_cast<BotFlyer*>(mainflyer.get()))-> start(); }
	for (int i = 0; i < bots.size(); i++) 
	{
		bots[i]->start();
	}
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
		if (bots[i]->crashed) { change_bot(i); }
	}
}

/**------------------------------------------------------------
  Asks all bots to act (decide whether they use engine).
  -----------------------------------------------------------*/
void Game::call_bots_action()
{
	if (config->PLAYER_IS_BOT) { (static_cast<BotFlyer*>(mainflyer.get()))->action(); }
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
			change_bot(i);
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
	double power = pow(r.module(), 3);
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
	while (true) 
	{
		unique_ptr<BotFlyer> f (new BotFlyer());
		double angle = d_random(0., M_PI * 2);
		f->position = Point(angle) * config->BOT_SCOPE + mainflyer->position;
		f->velocity = -Point(angle) * config->INIT_VELOCITY;
		if (!crashed(f->position, config->FLYER_SIZE))
		{
			bots[index] = std::move(f);
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

Game* game = nullptr;