#pragma once
#include <vector>
#include <memory>

#include "Star.h"
#include "Flyer.h"
#include "BotFlyer.h"
#include "Configuration.h"
#include "GalaxyObject.h"

using std::vector;
using std::unique_ptr;

/**------------------------------------------------------------
  This class is holding all the inner game logic: stores all
  the objects, moves them, adds and deletes.
  -----------------------------------------------------------*/
class Game
{
public:
	static Game& get();
	~Game(void);

	GalaxyObject flyer() const;
	int n_stars() const;
	int n_bots() const;
	GalaxyObject star(int i) const;
	GalaxyObject bot(int i) const;
	float distance() const;

	void start();
	void make_move();
	void call_bots_action();
	void user_turn_on_engine(char direction);
	void revise_stars();
	bool crashed(const Point& flyer_coord, float flyer_size) const;

	Point acceleration(const Point&  flyer_coord, const Point&  star_coord, float mass) const;
	Point summ_acceleration(const Point&  flyer_coord) const;

private:
	Game();

	vector<Star> _stars;
	vector<unique_ptr<BotFlyer>> _bots;
	unique_ptr<Flyer> _flyer;
	float _dist;

	void change_star(int index, bool initial);
	void change_bot(int index);

	bool no_star_collision(const Point&  obj_coord, float obj_size, int start_index, int end_index) const;
	bool in_sight_semisphere(const Point&  obj_coord) const;
};