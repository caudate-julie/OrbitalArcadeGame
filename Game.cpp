#include "Game.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Configuration.h"
#include "auxiliary.h"

/**------------------------------------------------------------
  Singleton it all!
  -----------------------------------------------------------*/
Game& Game::get()
{
	static Game instance;
	return instance;
}

/**------------------------------------------------------------
  Creates new game with a flyer in the middle and several
  stars in the field ~ 3 times bigger than screen.
  -----------------------------------------------------------*/
Game::Game()
{
	Configuration& conf = Configuration::get();

	this->_flyer = std::move(unique_ptr<Flyer>(new BotFlyer())); //std::make_unique<BotFlyer>(BotFlyer());
	this->_dist = 0;

	this->_stars.resize(conf.STAR_NUMBER);
	for (int i = 0; i < conf.STAR_NUMBER; i++)
	{
		this->change_star(i, true);
	}

	this->_bots.resize(conf.BOT_NUMBER);
	for (int i = 0; i < conf.BOT_NUMBER; i++)
	{
		this->change_bot(i);
	}
}

/**------------------------------------------------------------
  Destructor =)
  -----------------------------------------------------------*/
Game::~Game(void) { }

/**------------------------------------------------------------
  Getters for private fields.
  -----------------------------------------------------------*/
GalaxyObject  Game::flyer() const { return this->_flyer->info(); }
int Game::n_stars() const { return static_cast<int>(this->_stars.size()); }
int Game::n_bots() const { return static_cast<int>(this->_bots.size()); }
GalaxyObject  Game::star(int i) const { return this->_stars[i].info(); }
GalaxyObject  Game::bot(int i) const  { return this->_bots[i]->info(); }
float Game::distance() const { return this->_dist; }

/**------------------------------------------------------------
  Starts prediction threads for all bots.
  -----------------------------------------------------------*/
void Game::start()
{
	(static_cast<BotFlyer*>(this->_flyer.get()))->start();   // <-- WHILE FLYER IS BOT
	for (int i = 0; i < this->_bots.size(); i++) 
	{
		this->_bots[i]->start();
	}
}

/**------------------------------------------------------------
  Moves flyers to the next position.
  Should! also destroy crashed flyers.
  -----------------------------------------------------------*/
void Game::make_move()
{
	this->_flyer->move(this->summ_acceleration(this->_flyer->position));
	this->_dist += (this->_flyer->velocity.module());
	for (int i = 0; i < this->_bots.size(); i++) 
	{
		this->_bots[i]->move(this->summ_acceleration(this->_bots[i]->position));
	}
}

/**------------------------------------------------------------
  Asks all bots to act (decide whether they use engine).
  -----------------------------------------------------------*/
void Game::call_bots_action()
{
	(static_cast<BotFlyer*>(this->_flyer.get()))->action();      // <-- WHILE FLYER IS BOT
	for (int i = 0; i < _bots.size(); i++) { _bots[i]->action(); }
}

/**------------------------------------------------------------
  Adds engine (user-induced) acceleration to main flyer.
  Used by keyboard-processing function. Bots add up engines
  by themselves.
  -----------------------------------------------------------*/
void Game::user_turn_on_engine(char direction)
{
	this->_flyer->velocity += this->_flyer->engine_acceleration(direction);
}

/**------------------------------------------------------------
  Destroys every star and every bot that went ouf of scope
  (scopes differ for oblects). Creates a new object at the
  edge of scope instead of destroyed.
  -----------------------------------------------------------*/
void Game::revise_stars()
{
	Configuration& conf = Configuration::get();
	for (int i = 0; i < _stars.size(); i++)
	{
		if ((_stars[i].position - _flyer->position).module() > conf.STAR_SCOPE)
		{
			this->change_star(i, false);
		}
	}
	for (int i = 0; i < _bots.size(); i++)
	{
		if ((_bots[i]->position - _flyer->position).module() > 800)
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
	for (int i = 0; i < _stars.size(); i++)
	{
		if ((_stars[i].position - flyer_coord).module() < (_stars[i].size + flyer_size)) 
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
	for (Star s : this->_stars)
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
		float angle = f_random(0., M_PI * 2);
		Point unit(angle);

		// if we need a star towards flyer velocity, we may need another angle.
		if (!initial && !this->in_sight_semisphere(unit)) continue;

		Star s;
		float radius = (initial ? sqrt(f_random(0., 1.)) : f_random(0.8, 1.)) * conf.STAR_SCOPE;
		s.position = unit * radius + this->_flyer->position;
		if (this->no_star_collision(s.position, s.size, 0, index)
			&& (initial || this->no_star_collision(s.position, s.size, index, conf.STAR_NUMBER)))
		{ 
			this->_stars[index] = s;
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
		unique_ptr<BotFlyer> f (new BotFlyer());
		float angle = f_random(0., M_PI * 2);
		f->position = Point(angle) * 800 + this->_flyer->position;
		f->velocity = -Point(angle);
		if (this->no_star_collision(f->position, conf.FLYER_SIZE, 0, _stars.size()))
		{
			this->_bots[index] = std::move(f);
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
		float min_distance = Configuration::get().STAR_MIN_SPACE + obj_size + _stars[i].size;
		if ((_stars[i].position - obj_coord).module() < min_distance) { return false; }
	}
	return true;
}

/**------------------------------------------------------------
  For directional star addition: checks that vector from
  flyer to star makes sharp angle with flyer's velocity.
  -----------------------------------------------------------*/
bool Game::in_sight_semisphere(const Point& direction) const
{
	return (this->_flyer->velocity.x * direction.x 
		  + this->_flyer->velocity.y * direction.y) > 0;
}