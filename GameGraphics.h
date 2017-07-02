#pragma once
#include <SFML\Graphics.hpp>
#include "Game.h"
#include <string>
#include <memory>

class Star;
class Flyer;
using std::unique_ptr;

/**------------------------------------------------------------
  This class is responsible for all actual drawing of objects.

  Window member is public, because check whether it is open
  and player actions are processed in another class.
  
  ??? Could be shared_pointer instead ???
  -----------------------------------------------------------*/
class GameGraphics
{
public:
	static GameGraphics& get();
	~GameGraphics();

	sf::RenderWindow window;

	// whole screens
	void show_start_screen();
	void redraw_game_screen();
	void show_end_screen();
	
	// running debug output
	void show_message(std::string s);

private:
	GameGraphics();
//	static GameGraphics* instance;

	sf::Text text;
	sf::Font font;
	Point corner;            // join screen and galaxy position
	Point back_corner;       // same for background (other layer)
	std::string message;     // running debug output

	bool show_single_vector;
	bool show_all_vectors;

	// single element drawing
	void draw_star(const GalaxyObject);
	void draw_flyer(const GalaxyObject);
	void show_flyer_stats();
	void draw_single_vector();
	void draw_all_vectors();
	void draw_vector(const Point& p);

	Point get_screen_position(const Point& galaxy_coord, float size) const;
	void update_corner();
};

