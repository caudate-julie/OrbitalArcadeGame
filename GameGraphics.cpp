#include <sstream>

#include "Game.h"
#include "GameGraphics.h"
#include "Configuration.h"
#include "Background.h"
#include "auxiliary.h"

extern Background* background;
extern Configuration* config;
extern Game* game;

/**------------------------------------------------------------
  Creates new GameGraphics, sets up the window.
  -----------------------------------------------------------*/
GameGraphics::GameGraphics() 
{
	window.create(sf::VideoMode(config->WIDTH, config->HEIGHT), "Orbital");

	corner = Point(-config->WIDTH / 2, -config->HEIGHT / 2);
	
	font.loadFromFile("a_nova.ttf");
	text = sf::Text();
	text.setFont(font);
	text.setCharacterSize(14);
	text.setPosition(5, 5);
	message = "";

	show_acceleration_vector = true;

	star_render.create(50, 50);
	star_render.clear(sf::Color::Transparent);
	for (int i = 0; i < 1000; i++) 
	{ 
		Point p(d_random(0, asin(1) * 4));
		p *= d_random(0, 25);
		p += Point(25, 25);
		sf::Vertex v;
		v.position = p.vector();
		v.color = sf::Color(255, 255, 50, 255);
		star_render.draw(&v, 1, sf::Points);
	}
	star_render.display();

	flyershape = sf::CircleShape(6, 3);
//	flyershape.setScale(0.8f, 1.f);
	flyershape.setOrigin(3, 3);
}

/**------------------------------------------------------------
  Destructor =)
  -----------------------------------------------------------*/
GameGraphics::~GameGraphics() { }


/////////////////////
// Drawing screens //
/////////////////////

/**------------------------------------------------------------
  Start screen (nothing here).
  -----------------------------------------------------------*/
void GameGraphics::show_start_screen() {}

/**------------------------------------------------------------
  Main function for game screen redrawing every turn.
  -----------------------------------------------------------*/
void GameGraphics::redraw_game_screen()
{
	window.clear(sf::Color::Black);
	update_corner();

	background->draw();
	for (int i = 0; i < game->n_stars(); i++) { draw_star(game->star(i)); }
	for (int i = 0; i < game->n_bots(); i++) { draw_flyer(game->bot(i), 'B'); }
	draw_flyer(game->player(), 'M');

	show_flyer_stats();
	if (show_acceleration_vector) { draw_acceleration_vector(); }

	window.display();
}

/**------------------------------------------------------------
  End screen. Probably should be in separate class -
  too many elements.
  -----------------------------------------------------------*/
void GameGraphics::show_end_screen() 
{
	// STUB
	window.clear(sf::Color::Color(50, 50, 50, 0));
	window.display();
	sf::Clock end_time;
	while(end_time.getElapsedTime() < sf::milliseconds(500)) {}
	window.close();
}

/**------------------------------------------------------------
  Once again, this is auxiliary function to show running debug
  output. Every debug message holdsis shown here until next 
  debug message appears.
  -----------------------------------------------------------*/
void GameGraphics::show_message(std::string s)
{
	message = s;
}

/**============================================================
  ===================== PRIVATE MEMBERS =======================
  ===========================================================*/

/////////////////////////////////
// Drawing individual elements //
/////////////////////////////////

/**------------------------------------------------------------
  Draws a star.
  -----------------------------------------------------------*/
void GameGraphics::draw_star(const GalaxyObject star)
{
	Point p = get_screen_position(star.position) - Point(25, 25);
	sf::Sprite sp(star_render.getTexture());
	sp.setScale(static_cast<float>(star.size / 25), static_cast<float>(star.size / 25));
	sp.setPosition(p.vector());
	window.pushGLStates();
	window.draw(sp);
	window.popGLStates();
	}

/**------------------------------------------------------------
  Draws a flyer.
  -----------------------------------------------------------*/
void GameGraphics::draw_flyer(const GalaxyObject flyer, char type)
{
	switch (type)
	{
	case 'B': flyershape.setFillColor(sf::Color(150, 0, 150, 255)); break;
	case 'M': flyershape.setFillColor(sf::Color::Red); break;
	}
	
	Point p = get_screen_position(flyer.position);
	flyershape.setPosition(p.vector());
	flyershape.setRotation(0);
	double angle = acos(flyer.direction.x / flyer.direction.module());
	if (flyer.direction.y < 0) angle = -angle;
	flyershape.setRotation(static_cast<float>(angle / 3.141592 * 180 + 90));
	window.draw(flyershape);
}

/**------------------------------------------------------------
  Draws single vector.
  -----------------------------------------------------------*/
void GameGraphics::draw_acceleration_vector()
{
	Point p = game->summ_acceleration(game->player().position);
	Point position = game->player().position - corner;
	sf::Vertex line[] = { sf::Vertex(), sf::Vertex() };
	line[0].position = position.vector();
	line[0].color = sf::Color::White;
	position += p * 1000;
	line[1].position = position.vector();
	line[1].color = sf::Color::White;
	window.draw(line, 2, sf::Lines);
}

//////////////////////
// Auxiliary stuff  //
//////////////////////

/**------------------------------------------------------------
  Shows some stats (distance at the moment) and running debug
  output.
  -----------------------------------------------------------*/
void GameGraphics::show_flyer_stats()
{
	std::stringstream ss;
	ss << "distance: " << (int)(game->distance() / config->OUTPUT_DIST_COEFF) 
		<< "\n\n" << message;
	text.setString(ss.str());
	
	window.draw(text);
}

/**------------------------------------------------------------
  Common for star and flyer function for placing shape
  into the window (convert galaxy coordinates to screen and
  center - shapes are drawn from corner).
  -----------------------------------------------------------*/
Point GameGraphics::get_screen_position(const Point& galaxy_coord) const
{
	return galaxy_coord /*- Point(size / 2, size / 2)*/ - corner;
}

/**------------------------------------------------------------
  If flyer is going out of borders, screen moves with it.
  -----------------------------------------------------------*/
void GameGraphics::update_corner()
{
	Point p = get_screen_position(game->player().position);
	double x_move = 0, y_move = 0;
	if (p.x < config->MARGIN) { x_move = (p.x - config->MARGIN); }
	if (p.x > config->WIDTH - config->MARGIN) { x_move = (p.x - config->WIDTH + config->MARGIN); }
	if (p.y < config->MARGIN) { y_move = (p.y - config->MARGIN); }
	if (p.y > config->HEIGHT - config->MARGIN) { y_move = (p.y - config->HEIGHT + config->MARGIN); }
	corner += Point(x_move, y_move);
	background->corner += Point(x_move, y_move);
}


GameGraphics* gamegraphics = nullptr;