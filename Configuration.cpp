#include "Configuration.h"
#include <fstream>
#include <string>
#include <sstream>

/**------------------------------------------------------------
  Singletonus vulgaris.
  -----------------------------------------------------------*/
Configuration& Configuration::get()
{
	static Configuration instance;
	return instance;
}

/**------------------------------------------------------------
  Destructor.
  -----------------------------------------------------------*/
Configuration::~Configuration() { /*delete this->instance;*/ }


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
		STAR_ADD_SIZE(star_add_size_),

		STAR_REVISE_TIME(star_revise_time_),
		STAR_NUMBER(star_number_),
		STAR_SCOPE(star_scope_),
		STAR_DENSITY(star_density_),
		STAR_MIN_SPACE(star_min_space_),
		TIME_UNIT(time_unit_),
		G_CONST(g_const_),
		ENGINE(engine_),
		BOT_NUMBER(bot_number_),
		BOT_ACTION(bot_action_),
		OUTPUT_DIST_COEFF(output_dist_coeff_)
{
	this->default_values();
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
		std::stringstream s;
		std::stringstream value;
		int i = 0;
		while (i < line.size() && line[i] != ' ')
		{
			s << line[i++];
		}
		i++;
		while (i < line.size())
		{
			value << line[i++];
		}
		
		if (s.str() == "width_") { width_ = std::stoi(value.str()); }
		else if (s.str() == "height_") { height_ = std::stoi(value.str()); }
		else if (s.str() == "margin_") { margin_ = std::stoi(value.str()); }
		else if (s.str() == "flyer_size_") { flyer_size_ = std::stof(value.str()); }
		else if (s.str() == "star_min_size_") { star_min_size_ = std::stoi(value.str()); }
		else if (s.str() == "star_add_size_") { star_add_size_ = std::stoi(value.str()); }
		else if (s.str() == "star_density_") { star_density_ = std::stof(value.str()); }
		else if (s.str() == "star_min_space_") { star_min_space_ = std::stof(value.str()); }
		else if (s.str() == "star_number_") { star_number_ = std::stoi(value.str()); }
		else if (s.str() == "star_scope_") { star_scope_= std::stof(value.str()); }
		else if (s.str() == "time_unit_") { time_unit_ = std::stoi(value.str()); }
		else if (s.str() == "g_const_") { g_const_ = std::stof(value.str()); }
		else if (s.str() == "engine_") { engine_ = std::stof(value.str()); }
		else if (s.str() == "output_dist_coeff_") { output_dist_coeff_ = std::stof(value.str()); }
		else if (s.str() == "star_revise_time_") { star_revise_time_ = std::stoi(value.str()); }
	}
	conf_file.close();
}

/**------------------------------------------------------------
  Resets size of screen. Recalculates margin, also triggeres
  the method to recalculate star generation parameters.
  -----------------------------------------------------------*/
void Configuration::set_screen_size(int width, int height)
{
	this->width_ = width;
	this->height_ = height;
	this->margin_ = ((width < height) ? width : height) / 4;
	this->set_star_generation_consts();
}

/**------------------------------------------------------------
  Sets user-changeable constants, that affect game difficulty:
  time_unit (resets speed), engine power and gravity.
  -----------------------------------------------------------*/
void Configuration::set_game_difficulty(int time_unit, float engine, float g_const)
{
	this->time_unit_ = time_unit;
	this->engine_ = engine;
	this->g_const_ = g_const;
}

/**------------------------------------------------------------
  Given size of screen, recalculates number of stars generated
  at every time and distance, where one stars disappear and
  other appear.
  -----------------------------------------------------------*/
void Configuration::set_star_generation_consts()
{
	// !!!!! MAGICAL_NUMBER! 1.5 of screen diagonal
	this->star_scope_ = sqrt(width_ * width_ + height_ * height_) * 1.5;

	// how much linear space does one star need for average - proportional to max size.
	float living_space = (star_min_size_ + star_add_size_) * star_density_;
	this->star_number_ = pow(star_scope_ / living_space, 2);
}

/**------------------------------------------------------------
  This function fills in all the constants, that were not found
  in config file (actually does it before reading file).
  -----------------------------------------------------------*/
void Configuration::default_values()
{
	// objects' size
	flyer_size_ = 6;
	star_min_size_ = 10;
	star_add_size_ = 30;

	star_density_ = 6;
	star_min_space_ = 10;
	this->set_screen_size(800, 600);
	// star_number and star_dist are also calculated here.

	// speed and gravity settings
	time_unit_ = 5;
	g_const_ = 10;   // gravitation constant
	engine_ = 0.7;   // engine (user invervention)

	bot_number_ = 5;
	bot_action_ = 100;

	output_dist_coeff_ = 20;
	star_revise_time_ = 5;
}