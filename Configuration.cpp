#include <fstream>
#include <string>
#include <sstream>
#include "Configuration.h"

Configuration* config = nullptr;

/**------------------------------------------------------------
  Destructor.
  -----------------------------------------------------------*/
Configuration::~Configuration() { /*delete instance;*/ }

/**------------------------------------------------------------
  Pre-defined (default) settings.
  For now: are read from config.txt (if const is not mentioned
  there, it gets the default number).
  THIS IS A MESS! and needs to be rewritten.
  -----------------------------------------------------------*/
Configuration::Configuration()
	  : WIDTH(width_),
		HEIGHT(height_),
		MARGIN(margin_),

		FLYER_SIZE(flyer_size_),
		STAR_MIN_SIZE(star_min_size_),
		STAR_MAX_SIZE(star_max_size_),

		STAR_REVISE_TIME(star_revise_time_),
		STAR_NUMBER(star_number_),
		STAR_SCOPE(star_scope_),
		STAR_DENSITY(star_density_),
		STAR_MIN_SPACE(star_min_space_),

		TIME_UNIT(time_unit_),
		G_CONST(g_const_),
		ENGINE(engine_),
		INIT_VELOCITY(init_velocity_),

		BOT_NUMBER(bot_number_),
		BOT_ACTION(bot_action_),
		BOT_SCOPE(bot_scope_),
		BOT_MAX_STEPS(bot_max_steps_),
		PLAYER_IS_BOT(player_is_bot_),

		OUTPUT_DIST_COEFF(output_dist_coeff_)

{
	default_values();
	std::ifstream conf_file;
	try 
	{
		conf_file.open("config.txt");
	}
	catch (const std::exception& e)
	{
		// DO SMTH!
		return;
	}
	std::string line;
	while (getline(conf_file, line))
	{
		if (line.size() <= 1 || line[0] == '#') continue;    // <-- comment symbol
		std::stringstream key;
		std::stringstream value;
		int i = 0;
		while (i < line.size() && line[i] != ' ') { key << line[i++]; }
		i++;
		while (i < line.size()) { value << line[i++]; }

		if (key.str() == "width_") { width_ = std::stoi(value.str()); }
		else if (key.str() == "height_") { height_ = std::stoi(value.str()); }
		else if (key.str() == "margin_") { margin_ = std::stoi(value.str()); }

		else if (key.str() == "flyer_size_") { flyer_size_ = std::stod(value.str()); }
		else if (key.str() == "star_min_size_") { star_min_size_ = std::stod(value.str()); }
		else if (key.str() == "star_max_size_") { star_max_size_ = std::stod(value.str()); }

		else if (key.str() == "star_revise_time_") { star_revise_time_ = std::stoi(value.str()); }
		else if (key.str() == "star_density_") { star_density_ = std::stod(value.str()); }
		else if (key.str() == "star_min_space_") { star_min_space_ = std::stod(value.str()); }
		else if (key.str() == "star_number_") { star_number_ = std::stoi(value.str()); }
		else if (key.str() == "star_scope_") { star_scope_= std::stod(value.str()); }

		else if (key.str() == "time_unit_") { time_unit_ = std::stoi(value.str()); }
		else if (key.str() == "g_const_") { g_const_ = std::stod(value.str()); }
		else if (key.str() == "engine_") { engine_ = std::stod(value.str()); }
		else if (key.str() == "init_velocity_") { init_velocity_ = std::stod(value.str()); }

		else if (key.str() == "bot_number_") { bot_number_ = std::stoi(value.str()); }
		else if (key.str() == "bot_action_") { bot_action_ = std::stoi(value.str()); }
		else if (key.str() == "bot_scope_") { bot_scope_ = std::stod(value.str()); }
		else if (key.str() == "bot_max_steps_") { bot_max_steps_ = std::stoi(value.str()); }
		else if (key.str() == "player_is_bot_") { player_is_bot_ = (bool)std::stoi(value.str()); }

		else if (key.str() == "output_dist_coeff_") { output_dist_coeff_ = std::stod(value.str()); }
	}
	conf_file.close();
}

/**------------------------------------------------------------
  Resets size of screen. Recalculates margin, also triggeres
  the method to recalculate star generation parameters.
  -----------------------------------------------------------*/
void Configuration::set_screen_size(int width, int height)
{
	width_ = width;
	height_ = height;
	margin_ = ((width < height) ? width : height) / 4;
	set_star_generation_consts();
}

/**------------------------------------------------------------
  Sets user-changeable constants, that affect game difficulty:
  time_unit (resets speed), engine power and gravity.
  -----------------------------------------------------------*/
void Configuration::set_game_difficulty(int time_unit, double engine, double g_const)
{
	time_unit_ = time_unit;
	engine_ = engine;
	g_const_ = g_const;
}

void Configuration::set_bot_player(bool is_bot)
{
	player_is_bot_ = is_bot;
}

/**------------------------------------------------------------
  Given size of screen, recalculates number of stars generated
  at every time and distance, where one stars disappear and
  other appear.
  -----------------------------------------------------------*/
void Configuration::set_star_generation_consts()
{
	// !!!!! MAGICAL_NUMBER! 1.5 of screen diagonal
	star_scope_ = sqrt(width_ * width_ + height_ * height_) * 1.5;

	// how much linear space does one star need for average - proportional to max size.
	double living_space = (star_max_size_) * star_density_;
	star_number_ = static_cast<int>(pow(star_scope_ / living_space, 2));
}

/**------------------------------------------------------------
  This function fills in all the constants, that were not found
  in config file (actually does it before reading file).
  -----------------------------------------------------------*/
void Configuration::default_values()
{
	flyer_size_ = 6;
	star_min_size_ = 10;
	star_max_size_ = 30;

	star_density_ = 6;
	star_min_space_ = 10;
	set_screen_size(800, 600);
	// star_number and star_dist are also calculated here.

	// speed and gravity settings
	time_unit_ = 5;
	g_const_ = 10;   // gravitation constant
	engine_ = 0.7f;   // engine (user invervention)
	init_velocity_ = 2;

	bot_number_ = 5;
	bot_action_ = 10000;
	bot_scope_ = (width_ > height_) ? width_ : height_;
	bot_max_steps_ = 200;
	player_is_bot_ = false;

	output_dist_coeff_ = 20;
	star_revise_time_ = 5;
}

