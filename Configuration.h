#pragma once
#include <cmath>

class Configuration
{
public:
	//static Configuration& get();
	Configuration();
	~Configuration();

	// screen size
	const int& WIDTH;
	const int& HEIGHT;
	const int& MARGIN;     // when the screen starts moving. Flyer is never getting closer to edges.

	// objects' size
	const double& FLYER_SIZE;    // to centralize and determine collisions.
	const double& STAR_MIN_SIZE;
	const double& STAR_MAX_SIZE;

	// star generation
	const int& STAR_REVISE_TIME;     // frequency in milliseconds of recalculating.
	const int& STAR_NUMBER;          // number of generated stars (one goes out - other goes in)
	const double& STAR_SCOPE;         // star that is farther is deleted and a new is generated
	const double& STAR_DENSITY;       // to calculate number of stars for initial screen
	const double& STAR_MIN_SPACE;     // to generate stars apart from each other.

	// speed and gravity settings
	const int& TIME_UNIT;       // one time unit in milliseconds to move the flyer.
	const double& ENGINE;        // magnitude of keyboard speed addition
	const double& G_CONST;       // gravitation constant
	const double& INIT_VELOCITY;  // initial velocity. It probably affects overall speed.

	// bots
	const int& BOT_NUMBER;          // number of bots in game at any time.
	const double& BOT_SCOPE;         // bot that is farther is deleted and a new is generated
	const int& BOT_ACTION;          // in millisecs - time when bots are asked to do action.
	const int& BOT_MAX_STEPS;       // number of prediction steps
	const bool& PLAYER_IS_BOT;      // if true, player's flyer is self-operated.

	// other
	const double& OUTPUT_DIST_COEFF; // recalculate distance for conveniency of screen showing
	

	// reassignment functions
	void set_screen_size(int width, int height);
	void set_game_difficulty(int game_move, double engine, double g_const);

private:
	//static Configuration* instance;
	int width_;
	int height_;
	int margin_;

	double flyer_size_;
	double star_min_size_;
	double star_max_size_;

	int star_revise_time_;
	int star_number_;
	double star_scope_;
	double star_density_;
	double star_min_space_;

	int time_unit_;
	double engine_;
	double g_const_;
	double init_velocity_;

	int bot_number_;
	int bot_action_;
	double bot_scope_;
	int bot_max_steps_;
	bool player_is_bot_;

	double output_dist_coeff_;

	// inner auxiliary functions
	void default_values();
	void set_star_generation_consts();
};
