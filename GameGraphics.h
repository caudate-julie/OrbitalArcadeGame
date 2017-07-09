#pragma once
#include <SFML\Graphics.hpp>
#include <string>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>

#include "Point.h"
#include "GalaxyObject.h"
#include "QueuedForGraphics.h"

//class Star;
//class Flyer;

using std::unique_ptr;

/**------------------------------------------------------------
  This class is responsible for all actual drawing of objects.
  -----------------------------------------------------------*/
class GameGraphics
{
public:
	GameGraphics();
	~GameGraphics();

	std::queue<unique_ptr<QueuedForGraphics>> graphics_queue;
	std::mutex queue_mutex;

	void reset();

	// whole screens
	void show_start_screen();
	void redraw_game_screen();
	void reset_foreground();
	
	// running debug output
	void show_message(std::string s);

private:
	void show_end_screen(int dist);   // <-- stub - here must be the whole Achievement class
	
	sf::Text text;
	sf::Font font;
	Point screen_shift;            // joins screen and galaxy position
	Point foreground_shift;         // joins foreground stars and galaxy
	std::string message;           // running debug output
	bool show_acceleration_vector;

	std::thread foreground_redrawing_thread;
	std::mutex foreground_mutex;
	bool foreground_stop;

	sf::RenderTexture* foreground;
	sf::RenderTexture* secondary_foreground;
	//sf::Sprite* foreground_sprite;
	sf::CircleShape flyershape;

	// single element drawing
	void draw_stars();
	void draw_flyer(const GalaxyObject flyer, char type);
	void show_flyer_stats();
	void draw_acceleration_vector();

	//Point get_screen_position(const Point& galaxy_coord) const;
	void update_screen_shift();
};

