#pragma once
#include <SFML\Graphics.hpp>
#include <string>
#include <memory>

#include "Point.h"
#include "GalaxyObject.h"

//class Star;
//class Flyer;

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
	// static GameGraphics& get();
	GameGraphics();
	~GameGraphics();

	void reset();

	// whole screens
	void show_start_screen();
	void redraw_game_screen();
	
	// running debug output
	void show_message(std::string s);

private:
	void show_end_screen(int dist);   // <-- stub - here must be the whole Achievement class
	
	sf::Text text;
	sf::Font font;
	Point corner;            // joins screen and galaxy position
	Point foreground_corner;       // joins foreground stars and galaxy
	std::string message;     // running debug output
	bool show_acceleration_vector;

	sf::RenderTexture forestars;
	sf::CircleShape flyershape;

	// single element drawing
	void draw_stars();
	void draw_flyer(const GalaxyObject flyer, char type);
	void show_flyer_stats();
	void draw_acceleration_vector();

	void set_forestar_layer();
	Point get_screen_position(const Point& galaxy_coord) const;
	void update_corner();

};

