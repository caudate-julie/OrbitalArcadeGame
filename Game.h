#pragma once
#include <vector>
#include <memory>
#include <thread>

#include "Star.h"
#include "GalaxyObject.h"

class BotFlyer;
class Flyer;

using std::vector;
//using std::unique_ptr;

/**------------------------------------------------------------
  This class is holding all the inner game logic: stores all
  the objects, moves them, adds and deletes.
  -----------------------------------------------------------*/
class Game
{
public:
	Game();
	~Game(void);

	void reset();

	int n_stars() const;
	GalaxyObject player() const;
	int n_bots() const;
	GalaxyObject star(int i) const;
	GalaxyObject bot(int i) const;
	int distance() const;
	const bool& game_over;
	bool stop_threads;       //  <-- FRIEND PREDICTION
	vector<std::thread> bot_threads; //  <-- FRIEND PREDICTION
	
	void start();
	void make_move();
	void call_bots_action();
	void user_turn_on_engine(char direction);
	bool revise_stars();
	bool crashed(const Point& flyer_coord, double min_distance) const;

	Point acceleration(const Point&  flyer_coord, const Point&  star_coord, double mass) const;
	Point summ_acceleration(const Point&  flyer_coord) const;

private:
	vector<Star> stars;
	//vector<unique_ptr<BotFlyer>> bots;   // <-- UNIQUES TO POINTERS
	//unique_ptr<Flyer> mainflyer;         // <-- UNIQUES TO POINTERS
	vector<BotFlyer*> bots;                // <-- UNIQUES TO POINTERS
	Flyer* mainflyer;                      // <-- UNIQUES TO POINTERS
	double dist;
	bool player_crashed;

	void change_star(int index, bool initial);
	void change_bot(int index);

	bool in_sight_semisphere(const Point&  obj_coord) const;

	friend void predict(int i);
};