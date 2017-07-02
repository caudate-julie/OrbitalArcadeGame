#include <sstream>
#include "GameGraphics.h"
#include "Configuration.h"
#include "Background.h"
#

//GameGraphics* GameGraphics::instance = nullptr;

/**------------------------------------------------------------
  One more singleton.
  -----------------------------------------------------------*/
GameGraphics& GameGraphics::get()
{
	//if (!instance) { instance = new GameGraphics(); }
	static GameGraphics instance;
	return instance;
}

/**------------------------------------------------------------
  Creates new GameGraphics, sets up the window.
  -----------------------------------------------------------*/
GameGraphics::GameGraphics() 
{
	Configuration& conf = Configuration::get();
	this->window.create(sf::VideoMode(conf.WIDTH, conf.HEIGHT), "Orbital");
	
	this->font.loadFromFile("a_nova.ttf");

	this->text = sf::Text();
	this->text.setFont(font);
	this->text.setCharacterSize(14);
	this->text.setPosition(5, 5);

	this->message = "";

	this->show_single_vector = false;
	this->show_all_vectors = false;
}

/**------------------------------------------------------------
  Destructor =)
  -----------------------------------------------------------*/
GameGraphics::~GameGraphics()
{
	/*delete this->instance;*/
}


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
	Game& game = Game::get();
	window.clear(sf::Color::Black);
	this->update_corner();

	Background::get().draw();
	//for (Star s : game.stars) { draw_star(s); }
	for (int i = 0; i < game.n_stars(); i++) { draw_star(game.star(i)); }
	for (int i = 0; i < game.n_bots(); i++) { draw_flyer(game.bot(i)); }
	draw_flyer(game.flyer());

	this->show_flyer_stats();
	if (show_single_vector) { this->draw_single_vector(); }

	this->window.display();
}

/**------------------------------------------------------------
  End screen. Probably should be in separate class -
  too many elements.
  -----------------------------------------------------------*/
void GameGraphics::show_end_screen() 
{
	// STUB
	this->window.clear(sf::Color::Color(50, 50, 50, 0));
	this->window.display();
	sf::Clock end_time;
	while(end_time.getElapsedTime() < sf::milliseconds(500)) {}
	this->window.close();
}

/**------------------------------------------------------------
  Once again, this is auxiliary function to show running debug
  output. Every debug message holdsis shown here until next 
  debug message appears.
  -----------------------------------------------------------*/
void GameGraphics::show_message(std::string s)
{
	this->message = s;
}


/////////////////////////////////
// Drawing individual elements //
/////////////////////////////////

/**------------------------------------------------------------
  Draws a star.
  -----------------------------------------------------------*/
void GameGraphics::draw_star(const GalaxyObject star)
{
	sf::CircleShape s(star.size);
	s.setFillColor(sf::Color::Yellow);
	Point p = this->get_screen_position(star.position, star.size);
	s.setPosition(p.x, p.y);
	this->window.draw(s);
}

/**------------------------------------------------------------
  Draws a flyer.
  -----------------------------------------------------------*/
void GameGraphics::draw_flyer(const GalaxyObject flyer)
{
	sf::CircleShape s(flyer.size);
	s.setFillColor(sf::Color::Red);
	Point p = this->get_screen_position(flyer.position, flyer.size);
	s.setPosition(p.x, p.y);
	this->window.draw(s);
}

/**------------------------------------------------------------
  Draws summary acceleration vector.
  -----------------------------------------------------------*/
void GameGraphics::draw_single_vector()
{
	Game& game = Game::get();
	this->draw_vector(game.summ_acceleration(game.flyer().position));
}

/**------------------------------------------------------------
  Draws all acceleration vectors.
  -----------------------------------------------------------*/
void GameGraphics::draw_all_vectors()
{}

/**------------------------------------------------------------
  Draws single vector.
  -----------------------------------------------------------*/
void GameGraphics::draw_vector(const Point& p)
{
	Point position = Game::get().flyer().position - this->corner;
	sf::Vertex line[] = { sf::Vertex(), sf::Vertex() };
	line[0].position = sf::Vector2f(position.x, position.y);
	line[0].color = sf::Color::White;
	position += p * 1000;
	line[1].position = sf::Vector2f(position.x, position.y);
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
	Game& game = Game::get();
	Configuration& conf = Configuration::get();

	this->text = sf::Text();
	this->text.setFont(font);
	this->text.setCharacterSize(14);
	this->text.setPosition(5, 5);

	std::stringstream ss;
	/*ss << "X: " << (int)f->position.x << "\n\nY: " << (int)f->position.y
		<< "\n\nvel_X: " << (f->velocity.x * 100) 
		<< "\n\nvel_Y: " << (f->velocity.y * 100)
		<< "\n\nstars: " << this->game.stars.size() << "\n\n";*/
	ss << "distance: " << (int)(game.distance() / conf.OUTPUT_DIST_COEFF) 
		<< "\n\n" << message;
	this->text.setString(ss.str());
	
	this->window.draw(this->text);
}

/**------------------------------------------------------------
  Common for star and flyer function for placing shape
  into the window (convert galaxy coordinates to screen and
  center - shapes are drawn from corner).
  -----------------------------------------------------------*/
Point GameGraphics::get_screen_position(const Point& galaxy_coord, float size) const
{
	return galaxy_coord - Point(size, size) - this->corner;
}

/**------------------------------------------------------------
  If flyer is going out of borders, screen moves with it.
  -----------------------------------------------------------*/
void GameGraphics::update_corner()
{
	Configuration& conf = Configuration::get();
	Game& game = Game::get();
	Point p = this->get_screen_position(game.flyer().position, 0);
	float x_move = 0, y_move = 0;
	if (p.x < conf.MARGIN) { x_move = (p.x - conf.MARGIN); }
	if (p.x > conf.WIDTH - conf.MARGIN) { x_move = (p.x - conf.WIDTH + conf.MARGIN); }
	if (p.y < conf.MARGIN) { y_move = (p.y - conf.MARGIN); }
	if (p.y > conf.HEIGHT - conf.MARGIN) { y_move = (p.y - conf.HEIGHT + conf.MARGIN); }
	this->corner += Point(x_move, y_move);
	Background::get().corner += Point(x_move, y_move);
}
