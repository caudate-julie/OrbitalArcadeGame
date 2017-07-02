#include <sstream>
#include "GameGraphics.h"
#include "Configuration.h"
#include "Background.h"
#include "auxiliary.h"

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

	this->corner = Point(-conf.WIDTH / 2, -conf.HEIGHT / 2);
	
	this->font.loadFromFile("a_nova.ttf");

	this->text = sf::Text();
	this->text.setFont(font);
	this->text.setCharacterSize(14);
	this->text.setPosition(5, 5);

	this->message = "";

	this->show_single_vector = true;//false;
	this->show_all_vectors = false;


	this->star.create(50, 50);
	this->star.clear(sf::Color::Transparent);
	for (int i = 0; i < 1000; i++) 
	{ 
		Point p(f_random(0, asin(1) * 4));
		p *= f_random(0, 25);
		p += Point(25, 25);
		sf::Vertex v;
		v.position = p.vector();
		v.color = sf::Color(255, 255, 50, 255);
		this->star.draw(&v, 1, sf::Points);
	}
	this->star.display();

	this->flyershape = sf::CircleShape(10, 3);
	this->flyershape.setScale(0.8, 1);
	this->flyershape.rotate(asin(1));
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
	for (int i = 0; i < game.n_bots(); i++) { draw_flyer(game.bot(i), 'B'); }
	draw_flyer(game.flyer(), 'M');

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
//	sf::CircleShape s(static_cast<float>(star.size));
//	s.setFillColor(sf::Color::Yellow);
	Point p = this->get_screen_position(star.position, star.size);
	sf::Sprite s = sf::Sprite(this->star.getTexture());
	s.setScale(star.size / 50, star.size / 50);
	s.setPosition(p.vector());
	this->window.draw(s);
}

/**------------------------------------------------------------
  Draws a flyer.
  -----------------------------------------------------------*/
void GameGraphics::draw_flyer(const GalaxyObject flyer, char type)
{
	//sf::CircleShape s(static_cast<float>(flyer.size));
	switch (type)
	{
	case 'B': this->flyershape.setFillColor(sf::Color(150, 0, 150, 255)); break;
	case 'M': this->flyershape.setFillColor(sf::Color::Red); break;
	}
	
	Point p = this->get_screen_position(flyer.position, flyer.size);
	this->flyershape.setPosition(p.vector());
	this->flyershape.setRotation(0);
	//this->flyershape.rotate(this->flyershape.getRotation());
	double angle = acos(flyer.direction.x / flyer.direction.module());
	if (flyer.direction.y < 0) angle = -angle;
	this->flyershape.setRotation(angle / 3.141592 * 180 + 90);
	this->window.draw(this->flyershape);
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
Point GameGraphics::get_screen_position(const Point& galaxy_coord, double size) const
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
	double x_move = 0, y_move = 0;
	if (p.x < conf.MARGIN) { x_move = (p.x - conf.MARGIN); }
	if (p.x > conf.WIDTH - conf.MARGIN) { x_move = (p.x - conf.WIDTH + conf.MARGIN); }
	if (p.y < conf.MARGIN) { y_move = (p.y - conf.MARGIN); }
	if (p.y > conf.HEIGHT - conf.MARGIN) { y_move = (p.y - conf.HEIGHT + conf.MARGIN); }
	this->corner += Point(x_move, y_move);
	Background::get().corner += Point(x_move, y_move);
}
