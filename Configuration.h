#pragma once
#include <cmath>

class Configuration
{
public:
	static Configuration& get();
	~Configuration();

	// screen size
	const int& WIDTH;
	const int& HEIGHT;
	const int& MARGIN;     // when the screen starts moving. Flyer is never getting closer to edges.

	// objects' size
	const float& FLYER_SIZE;    // to centralize and determine collisions.
	const float& STAR_MIN_SIZE;
	const float& STAR_MAX_SIZE;   // rand() % ADD_SIZE + MIN_SIZE

	// star generation
	const int& STAR_REVISE_TIME;     // frequency in milliseconds of recalculating.
	const int& STAR_NUMBER;          // number of generated stars (one goes out - other goes in)
	const float& STAR_SCOPE;         // star that is farther is deleted and a new is generated
	const float& STAR_DENSITY;       // to calculate number of stars for initial screen
	const float& STAR_MIN_SPACE;     // to generate stars apart from each other.

	// speed and gravity settings
	const int& TIME_UNIT;       // one time unit in milliseconds to move the flyer.
	const float& ENGINE;        // magnitude of keyboard speed addition
	const float& G_CONST;       // gravitation constant

	// bots
	const int& BOT_NUMBER;          // number of bots in game at any time.
	const int& BOT_ACTION;          // in millisecs - time when bots are asked to do action.

	// other
	const float& OUTPUT_DIST_COEFF; // recalculate distance for conveniency of screen showing
	

	// reassignment functions
	void set_screen_size(int width, int height);
	void set_game_difficulty(int game_move, float engine, float g_const);

private:
	//static Configuration* instance;
	Configuration();

	int width_;
	int height_;
	int margin_;

	float flyer_size_;
	float star_min_size_;
	float star_max_size_;

	int star_revise_time_;
	int star_number_;
	float star_scope_;
	float star_density_;
	float star_min_space_;

	int time_unit_;
	float engine_;
	float g_const_;

	int bot_number_;
	int bot_action_;

	float output_dist_coeff_;

	// inner auxiliary functions
	void default_values();
	void set_star_generation_consts();
};