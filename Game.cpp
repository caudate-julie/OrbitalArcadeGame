#include "Game.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Configuration.h"
#include <random>

/**------------------------------------------------------------
  Single game instance.
  -----------------------------------------------------------*/
Game* Game::instance = nullptr;

/**------------------------------------------------------------
  Singleton it all!
  -----------------------------------------------------------*/
Game& Game::get()
{
	if (!instance) { 
		instance = new Game(); 
	}
	return *instance;
}

/**------------------------------------------------------------
  Creates new game with a flyer in the middle and several
  stars in the field ~ 3 times bigger than screen.
  -----------------------------------------------------------*/
Game::Game()
{
	Configuration& conf = Configuration::get();

	this->flyer = new BotFlyer();
	this->distance = 0;

	this->stars.resize(conf.STAR_NUMBER);
	for (int i = 0; i < conf.STAR_NUMBER; i++)
	{
		this->change_star(i, true);
	}

	this->bots.resize(conf.BOT_NUMBER);
	for (int i = 0; i < conf.BOT_NUMBER; i++)
	{
		this->change_bot(i);
	}
}

/**------------------------------------------------------------
  Destructor =)
  -----------------------------------------------------------*/
Game::~Game(void)
{
	delete this->flyer;
}

/**------------------------------------------------------------
  Starts prediction threads for all bots.
  -----------------------------------------------------------*/
void Game::start()
{
	((BotFlyer*)this->flyer)->start();   // <-- WHILE FLYER IS BOT
	for (BotFlyer* f : this->bots)
	{
		f->start();
	}
}

/**------------------------------------------------------------
  Moves flyers to the next position.
  Should! also destroy crashed flyers.
  -----------------------------------------------------------*/
void Game::make_move()
{
	this->flyer->move(this->summ_acceleration(this->flyer->position));
	this->distance += (this->flyer->velocity.module());
	for (BotFlyer* f : bots)
	{
		f->move(this->summ_acceleration(f->position));
	}
}

/**------------------------------------------------------------
  Asks all bots to act (decide whether they use engine).
  -----------------------------------------------------------*/
void Game::call_bots_action()
{
	((BotFlyer*)this->flyer)->action();       // <-- WHILE FLYER IS BOT
	for (BotFlyer* f : bots) { f->action(); }
}

/**------------------------------------------------------------
  Adds engine (user-induced) acceleration to main flyer.
  Used by keyboard-processing function. Bots add up engines
  by themselves.
  -----------------------------------------------------------*/
void Game::user_turn_on_engine(char direction)
{
	this->flyer->velocity += this->flyer->engine_acceleration(direction);
}

/**------------------------------------------------------------
  Destroys every star and every bot that went ouf of scope
  (scopes differ for oblects). Creates a new object at the
  edge of scope instead of destroyed.
  -----------------------------------------------------------*/
void Game::revise_stars()
{
	Configuration& conf = Configuration::get();
	for (int i = 0; i < stars.size(); i++)
	{
		if ((stars[i].position - flyer->position).module() > conf.STAR_SCOPE)
		{
			this->change_star(i, false);
		}
	}
	for (int i = 0; i < bots.size(); i++)
	{
		if ((bots[i]->position - flyer->position).module() > 800)
		{
			this->change_bot(i);
		}
	}
}

/**------------------------------------------------------------
  Checks if the given point lies inside a star - for flyer
  means that it is crashed (for main flyer - that game's over).
  -----------------------------------------------------------*/
bool Game::crashed(const Point& flyer_coord, float flyer_size) const
{
	Configuration& conf = Configuration::get();
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
Point Game::acceleration(const Point& flyer_coord, const Point& star_coord, float mass) const
{
	Point r = flyer_coord - star_coord;
	float power = pow(r.module(), 3);
	float C = - mass / pow(r.module(), 3) * Configuration::get().G_CONST;
	Point p = r * C;
	return r * (- mass / pow(r.module(), 3) * Configuration::get().G_CONST);
}

/**------------------------------------------------------------
  Calculates and returns summ of accelerations towards all 
  the stars in the scope.
  -----------------------------------------------------------*/
Point Game::summ_acceleration(const Point& flyer_coord) const
{
	Point a;
	for (Star s : this->stars)
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
	Configuration& conf = Configuration::get();
	while (true) 
	{
		float angle = (float)((float)(rand() % 10000) / 10000 * 2 * M_PI);
		Point unit(angle);

		// if we need a star towards flyer velocity, we may need another angle.
		if (!initial && !this->in_sight_semisphere(unit)) continue;

		Star s;
		float radius = (float)((initial ? (sqrt(rand() % 10000) / 100) : 1) * conf.STAR_SCOPE);
		s.position = unit * radius + this->flyer->position;
		if (this->no_star_collision(s.position, s.size, 0, index)
			&& (initial || this->no_star_collision(s.position, s.size, index, conf.STAR_NUMBER)))
		{ 
			this->stars[index] = s;
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
	Configuration& conf = Configuration::get();
	while (true) {
		BotFlyer* f = new BotFlyer();
		float angle = (float)((double)(rand() % 10000) / 10000 * 2 * M_PI);
		f->position = Point(angle) * 800 + this->flyer->position;
		f->velocity = -Point(angle);
		if (this->no_star_collision(f->position, conf.FLYER_SIZE, 0, stars.size()))
		{
			this->bots[index] = f;
			return;
		}
	}
}

/**------------------------------------------------------------
  Checks that object with given position and size does not
  overlap with stars int [start_index, end_index] slice.
  -----------------------------------------------------------*/
bool Game::no_star_collision(const Point& obj_coord, float obj_size, int start_index, int end_index) const
{
	for (int i = start_index; i < end_index; i++)
	{
		if (
			(stars[i].position - obj_coord).module() 
			< (Configuration::get().STAR_MIN_SPACE + obj_size + stars[i].size)
			) 
		{ return false; }
	}
	return true;
}

/**------------------------------------------------------------
  For directional star addition: checks that vector from
  flyer to star makes sharp angle with flyer's velocity.
  -----------------------------------------------------------*/
bool Game::in_sight_semisphere(const Point& direction) const
{
	return (this->flyer->velocity.x * direction.x 
		  + this->flyer->velocity.y * direction.y) > 0;
}