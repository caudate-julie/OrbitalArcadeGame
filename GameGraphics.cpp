#include <sstream>

#include "Game.h"
#include "GameGraphics.h"
#include "Configuration.h"
#include "Background.h"
#include "auxiliary.h"
#include "InterimScreen.h"

extern Background* background;
extern Configuration* config;
extern Game* game;
extern sf::RenderWindow* window;

/**------------------------------------------------------------
  Creates new GameGraphics, sets up the window->
  -----------------------------------------------------------*/
GameGraphics::GameGraphics() 
{
	font.loadFromFile("a_nova.ttf");
	text = sf::Text();
	text.setFont(font);
	text.setCharacterSize(14);
	text.setPosition(-config->WIDTH / 2 + 5, -config->HEIGHT / 2 + 5);

	show_acceleration_vector = true;

	flyershape = sf::CircleShape(6, 3);
//	flyershape.setScale(0.8f, 1.f);
	flyershape.setOrigin(6, 6);
}

/**------------------------------------------------------------
  Destructor =)
  -----------------------------------------------------------*/
GameGraphics::~GameGraphics() { }


void GameGraphics::reset()
{
	screen_shift = Point(0, 0);
	forestars_shift = Point(0, 0);
	message = "";
	reset_forestar_layer();

	while(!graphics_queue.empty()) { graphics_queue.pop(); }
}

/////////////////////
// Drawing screens //
/////////////////////

/**------------------------------------------------------------
  Start screen (nothing here).
  -----------------------------------------------------------*/
void GameGraphics::show_start_screen() 
{
	show_end_screen(-1);
}

/**------------------------------------------------------------
  Main function for game screen redrawing every turn.
  -----------------------------------------------------------*/
void GameGraphics::redraw_game_screen()
{
	window->clear(sf::Color::Black);
	update_screen_shift();

	background->draw();
	draw_stars(); 

	for (int i = 0; i < game->n_bots(); i++) { draw_flyer(game->bot(i), 'B'); }
	draw_flyer(game->player(), 'M');

	show_flyer_stats();
	if (show_acceleration_vector) { draw_acceleration_vector(); }

	window->display();
}

void GameGraphics::reset_forestar_layer()
{
	forestars.create(config->WIDTH * 3, config->HEIGHT * 3);
	forestars.clear(sf::Color::Transparent);
	
	sf::View fv (forestars.getView());
	fv.move(-config->WIDTH * 1.5, -config->HEIGHT * 1.5);
	forestars.setView(fv);

	for (int i = 0; i < game->n_stars(); i++)
	{
		// right now stars are back to circles - that makes them in place
		sf::CircleShape star_shadow(game->star(i).size);
		star_shadow.setOrigin(game->star(i).size, game->star(i).size);
		star_shadow.setFillColor(sf::Color(100, 100, 0));
		star_shadow.setPosition(
			game->star(i).position.x - screen_shift.x,
			game->star(i).position.y - screen_shift.y
			);
		forestars.draw(star_shadow);
		// ...and no need to set them right until we start to really generate them
	}
	forestars_shift = screen_shift;
	forestars.display();
}

/**------------------------------------------------------------
  End screen. Probably should be in separate class -
  too many elements.
  -----------------------------------------------------------*/
void GameGraphics::show_end_screen(int dist) 
{
	InterimScreen startscreen;
	startscreen.show(dist);	
	
	//window->clear(sf::Color::Color(50, 50, 50, 0));
	//window->display();
	//sf::Clock end_time;
	//while(end_time.getElapsedTime() < sf::milliseconds(500)) {}
	//window->close();
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
void GameGraphics::draw_stars()
{
	sf::Sprite sprite (forestars.getTexture());
	sprite.setOrigin(
		sprite.getGlobalBounds().width / 2 - forestars_shift.x + screen_shift.x, 
		sprite.getGlobalBounds().height / 2 - forestars_shift.y + screen_shift.y
		);
	//sprite.setOrigin(config->WIDTH * 1.5, config->HEIGHT * 1.5);
	window->draw(sprite);
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
	
	Point p = flyer.position - screen_shift;
	flyershape.setPosition(p.vector());
	flyershape.setRotation(0);
	double angle = acos(flyer.direction.x / flyer.direction.module());
	if (flyer.direction.y < 0) angle = -angle;
	flyershape.setRotation(static_cast<float>(angle / 3.141592 * 180 + 90));
	window->draw(flyershape);
}

/**------------------------------------------------------------
  Draws single vector.
  -----------------------------------------------------------*/
void GameGraphics::draw_acceleration_vector()
{
	Point p = game->summ_acceleration(game->player().position);
	Point position = game->player().position - screen_shift;
	sf::Vertex line[] = { sf::Vertex(), sf::Vertex() };
	line[0].position = position.vector();
	line[0].color = sf::Color::White;
	position += p * 1000;
	line[1].position = position.vector();
	line[1].color = sf::Color::White;
	window->draw(line, 2, sf::Lines);
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
	ss << "distance: " << (int)(game->distance()) 
		<< "\n\n" << message;
	text.setString(ss.str());
	
	window->draw(text);
}

/**------------------------------------------------------------
  Common for star and flyer function for placing shape
  into the window (convert galaxy coordinates to screen and
  center - shapes are drawn from screen_shift).
  -----------------------------------------------------------*/
/*Point GameGraphics::get_screen_position(const Point& galaxy_coord) const
{
	return galaxy_coord - screen_shift;
}*/

/**------------------------------------------------------------
  If flyer is going out of borders, screen moves with it.
  -----------------------------------------------------------*/
void GameGraphics::update_screen_shift()
{
	Point p = game->player().position - screen_shift;
	double x_move = 0, y_move = 0;
	double width_margin = config->WIDTH / 2 - config->MARGIN;
	double height_margin = config->HEIGHT / 2 - config->MARGIN;

	if (p.x < - width_margin) { x_move = (p.x + width_margin); }
	if (p.x > width_margin) { x_move = (p.x - width_margin); }
	if (p.y < - height_margin) { y_move = (p.y + height_margin); }
	if (p.y > height_margin) { y_move = (p.y - height_margin); }
	Point move(x_move, y_move);
	screen_shift += move;
	//forestars_shift += move;
	background->back_shift += move;
}


GameGraphics* gamegraphics = nullptr;