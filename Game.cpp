#include "Game.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>

#include "Configuration.h"
#include "auxiliary.h"

extern Configuration* config;

/**------------------------------------------------------------
  Creates new game with a flyer in the middle and several
  stars in the field ~ 3 times bigger than screen.
  -----------------------------------------------------------*/
Game::Game()
{
	stars.resize(config->STAR_NUMBER);
	for (int i = 0; i < config->STAR_NUMBER; i++) 
	{ 
		change_star(i, true); 
	}

	bots.resize(config->BOT_NUMBER);
	for (int i = 0; i < config->BOT_NUMBER; i++)
	{ 
		change_bot(i); 
	}

	main_is_bot = true;
	flyer = std::move(unique_ptr<Flyer>(main_is_bot ? new BotFlyer() : new Flyer()));
	dist = 0;
}

/**------------------------------------------------------------
  Destructor =)
  -----------------------------------------------------------*/
Game::~Game(void) 
{
     //for (int i = 0; i < bots.size(); i++) bots[i]->stop();
}

/**------------------------------------------------------------
  Getters for private fields.
  -----------------------------------------------------------*/
GalaxyObject  Game::player() const     { return flyer->info(); }
int Game::n_stars() const              { return static_cast<int>(stars.size()); }
int Game::n_bots() const               { return static_cast<int>(bots.size()); }
GalaxyObject  Game::star(int i) const  { return stars[i].info(); }
GalaxyObject  Game::bot(int i) const   { return bots[i]->info(); }
double Game::distance() const          { return dist; }

/**------------------------------------------------------------
  Starts prediction threads for all bots.
  -----------------------------------------------------------*/
void Game::start()
{
	if (main_is_bot) { (static_cast<BotFlyer*>(flyer.get()))-> start(); }
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
	flyer->move(summ_acceleration(flyer->position));
	dist += (flyer->velocity.module());
	for (int i = 0; i < bots.size(); i++) 
	{
		bots[i]->move(summ_acceleration(bots[i]->position));
	}
}

/**------------------------------------------------------------
  Asks all bots to act (decide whether they use engine).
  -----------------------------------------------------------*/
void Game::call_bots_action()
{
	if (main_is_bot) { (static_cast<BotFlyer*>(flyer.get()))->action(); }
	for (int i = 0; i < bots.size(); i++) { bots[i]->action(); }
}

/**------------------------------------------------------------
  Adds engine (user-induced) acceleration to main flyer.
  Used by keyboard-processing function. Bots add up engines
  by themselves.
  -----------------------------------------------------------*/
void Game::user_turn_on_engine(char direction)
{
	flyer->velocity += flyer->engine_acceleration(direction);
}

/**------------------------------------------------------------
  Destroys every star and every bot that went ouf of scope
  (scopes differ for oblects). Creates a new object at the
  edge of scope instead of destroyed.
  -----------------------------------------------------------*/
void Game::revise_stars()
{
	for (int i = 0; i < stars.size(); i++)
	{
		if ((stars[i].position - flyer->position).module() > config->STAR_SCOPE)
		{
			change_star(i, false);
		}
	}
	for (int i = 0; i < bots.size(); i++)
	{
		if ((bots[i]->position - flyer->position).module() > config->BOT_SCOPE)
		{
			change_bot(i);
		}
	}
}

/**------------------------------------------------------------
  Checks if the main flyer crashed into some star (which means
   game is over).
  -----------------------------------------------------------*/
bool Game::crashed() const
{
	return crashed(flyer->position, config->FLYER_SIZE);
}

/**------------------------------------------------------------
  Checks if the given point (+- size) lies inside a star - 
  for flyer means that it is crashed.
  -----------------------------------------------------------*/
bool Game::crashed(const Point& flyer_coord, double flyer_size) const
{
	for (int i = 0; i < stars.size(); i++)
	{
		if ((stars[i].position - flyer_coord).module() < (stars[i].size + flyer_size)) 
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
	double C = - mass / pow(r.module(), 3) * config->G_CONST;
	Point p = r * C;
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
  Initial sets the mode of adding star: initial star is added
  anywhere in the scope (in-game - only at the end of it),
  might not be towards flyer's velocity and only checks the
  collision with previous stars.
  -----------------------------------------------------------*/
void Game::change_star(int index, bool initial)
{
	while (true) 
	{
		double angle = d_random(0., M_PI * 2);
		Point unit(angle);

		// if we need a star towards flyer velocity, we may need another angle.
		if (!initial && !in_sight_semisphere(unit)) continue;

		Star s;
		double radius = (initial ? sqrt(d_random(0., 1.)) : d_random(0.8, 1.)) * config->STAR_SCOPE;
		s.position = unit * radius + flyer->position;
		if (no_star_collision(s.position, s.size, 0, index)
			&& (initial || no_star_collision(s.position, s.size, index, config->STAR_NUMBER)))
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
	//Configuration& conf = Configuration::get();
	while (true) {
		unique_ptr<BotFlyer> f (new BotFlyer());
		double angle = d_random(0., M_PI * 2);
		f->position = Point(angle) * config->BOT_SCOPE + flyer->position;
		f->velocity = -Point(angle) * config->INIT_VELOCITY;
		if (no_star_collision(f->position, config->FLYER_SIZE, 0, stars.size()))
		{
			bots[index] = std::move(f);
			return;
		}
	}
}

/**------------------------------------------------------------
  Checks that object with given position and size does not
  overlap with stars int [start_index, end_index] slice.
  -----------------------------------------------------------*/
bool Game::no_star_collision(const Point& obj_coord, double obj_size, int start_index, int end_index) const
{
	for (int i = start_index; i < end_index; i++)
	{
		double mindistance = config->STAR_MIN_SPACE + obj_size + stars[i].size;
		if ((stars[i].position - obj_coord).module() < mindistance) { return false; }
	}
	return true;
}

/**------------------------------------------------------------
  For directional star addition: checks that vector from
  flyer to star makes sharp angle with flyer's velocity.
  -----------------------------------------------------------*/
bool Game::in_sight_semisphere(const Point& direction) const
{
	return (flyer->velocity.x * direction.x 
		  + flyer->velocity.y * direction.y) > 0;
}

Game* game = nullptr;