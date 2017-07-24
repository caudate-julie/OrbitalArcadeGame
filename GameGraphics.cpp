#include <sstream>

#include "Game.h"
#include "GameGraphics.h"
#include "Configuration.h"
#include "Background.h"
#include "auxiliary.h"
#include "MenuScreen.h"
#include "Logger.h"

extern Background* background;
extern Configuration* config;
extern Game* game;
extern sf::RenderWindow* window;
extern Logger* logger;

/////////////////////////////////////////
//      Constructor, destructor,       //
//      interim resets                 //
/////////////////////////////////////////

/**------------------------------------------------------------
  Creates new GameGraphics, sets up reused characteristics.
  -----------------------------------------------------------*/
GameGraphics::GameGraphics() 
	: foreground_redrawing_thread()
{
	font.loadFromFile("a_nova.ttf");
	text = sf::Text();
	text.setFont(font);
	text.setCharacterSize(14);

	show_acceleration_vector = true;

	flyershape = sf::CircleShape(config->FLYER_SIZE, 3);
	flyershape.setOrigin(config->FLYER_SIZE, config->FLYER_SIZE);

	foreground = new sf::RenderTexture();
	secondary_foreground = new sf::RenderTexture();
	reset_screen_size_settings();
}

/**------------------------------------------------------------
  Destructor: stops thread, deletes texture pointers.
  -----------------------------------------------------------*/
GameGraphics::~GameGraphics() 
{
	foreground_stop = true;
	if (foreground_redrawing_thread.joinable()) { foreground_redrawing_thread.join(); }
	delete foreground;
	delete secondary_foreground;
}

/**------------------------------------------------------------
  Resets characteristics before every new game.
  -----------------------------------------------------------*/
void GameGraphics::reset()
{
	foreground_stop = true;
	if (foreground_redrawing_thread.joinable()) { foreground_redrawing_thread.join(); }

	// textures should be recreated before every game - or they behave STRANGE.
	foreground->create(config->WIDTH * 3, config->HEIGHT * 3);
	secondary_foreground->create(config->WIDTH * 3, config->HEIGHT * 3);
	sf::View fv (foreground->getDefaultView());
	fv.move(
		static_cast<float>(-config->WIDTH * 1.5), 
		static_cast<float>(-config->HEIGHT * 1.5)
		);
	foreground->setView(fv);
	secondary_foreground->setView(fv);

	screen_shift = Point(0, 0);
	foreground_shift = Point(0, 0);
	message = "";

	while(!graphics_queue.empty()) { graphics_queue.pop(); }

	foreground_stop = false;
	foreground_redrawing_thread.swap(std::thread(&GameGraphics::reset_foreground, this));
}

/**------------------------------------------------------------
  Constant settings that should change if window sizes changed.
  Resetting the window itself is done in main thread.
  Also textures are recreated at reset. Window size is only
  changed between games, so reset will be called after it.
  -----------------------------------------------------------*/
void GameGraphics::reset_screen_size_settings()
{
	text.setPosition(
		static_cast<float>(-config->WIDTH / 2 + 5), 
		static_cast<float>(-config->HEIGHT / 2 + 5)
		);
}

/////////////////////////////////////////
//         Drawing screens             //
/////////////////////////////////////////

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
	//logger->start("window cleared");
	window->clear(sf::Color::Black);
	//logger->stop("window cleared");

	update_screen_shift();

	//logger->start("background drawing");
	background->draw();
	//logger->stop("background drawing");

	//logger->start("stars drawing");
	draw_stars(); 
	//logger->stop("stars drawing");

	//logger->start("flyers drawing");
	for (int i = 0; i < game->n_bots(); i++) { draw_flyer(game->bot(i), 'B'); }
	draw_flyer(game->player(), 'M');
	//logger->stop("flyers drawing");

	show_flyer_stats();
	if (show_acceleration_vector) { draw_acceleration_vector(); }

	draw_star_indicators();
	
	//logger->start("window displayed");
	window->display();
	//logger->stop("window displayed");
}

/**------------------------------------------------------------
  In-thread function to redraw foreground (ingame stars).
  Should sleep, but right now doesn't.
  Except for creating / deleting, this is the only function to
  work with secondary foreground.
  -----------------------------------------------------------*/
void GameGraphics::reset_foreground()
{
	while (!foreground_stop)
	{
		//logger->start("foreground cycle");
		Point new_shift = screen_shift;  // screen shift may change while redrawing goes
		//logger->start("foreground reset");
		secondary_foreground->clear(sf::Color::Transparent);

		for (int i = 0; i < game->n_stars(); i++)
		{
			// right now stars are back to circles - that puts them in the right place...
			sf::CircleShape star_shadow(static_cast<float>(game->star(i).size));
			star_shadow.setOrigin(
				static_cast<float>(game->star(i).size), 
				static_cast<float>(game->star(i).size)
				);
			star_shadow.setFillColor(sf::Color(200, 200, 50));
			star_shadow.setPosition(
				static_cast<float>(game->star(i).position.x - new_shift.x),
				static_cast<float>(game->star(i).position.y - new_shift.y)
				);
			secondary_foreground->draw(star_shadow);
		}
		secondary_foreground->display();
		//logger->stop("foreground reset");

		std::lock_guard<std::mutex> lock_foreground(foreground_mutex);
		sf::RenderTexture* temp = foreground;
		foreground = secondary_foreground;
		secondary_foreground = temp;

		foreground_shift = new_shift;
		//logger->stop("foreground cycle");
	}
}

/**------------------------------------------------------------
  End screen (nothing here as well).
  -----------------------------------------------------------*/
void GameGraphics::show_end_screen(int dist) 
{
	MenuScreen startscreen;
	startscreen.show(dist);	
	
	//window->clear(sf::Color::Color(50, 50, 50, 0));
	//window->display();
	//sf::Clock end_time;
	//while(end_time.getElapsedTime() < sf::milliseconds(500)) {}
	//window->close();
}

/**------------------------------------------------------------
  Auxiliary function to show running debug output. 
  Every debug message is shown here until the next 
  debug message appears.
  -----------------------------------------------------------*/
void GameGraphics::show_message(std::string s)
{
	message = s;
}

/**============================================================
  ===================== PRIVATE MEMBERS =======================
  ===========================================================*/

/////////////////////////////////////////
//     Drawing individual elements     //
/////////////////////////////////////////

/**------------------------------------------------------------
  Draws foreground layer with ingame stars.
  -----------------------------------------------------------*/
void GameGraphics::draw_stars()
{
	//logger->start("mutex set");
	std::lock_guard<std::mutex> lock_foreground(foreground_mutex);
	//logger->stop("mutex set");

	sf::Sprite foresprite = sf::Sprite(foreground->getTexture());
	Point current_shift = foreground_shift - screen_shift;

	//logger->start("sprite drawing");
	foresprite.setOrigin(
		static_cast<float>(foresprite.getGlobalBounds().width / 2 - current_shift.x), 
		static_cast<float>(foresprite.getGlobalBounds().height / 2 - current_shift.y)
		);
	
	window->draw(foresprite);
	//logger->stop("sprite drawing");
}

/**------------------------------------------------------------
  Draws a flyer. Char type defines color: red for player ('M'),
  purple for bots ('B').
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
  Draws acceleration vector for main flyer.
  -----------------------------------------------------------*/
void GameGraphics::draw_acceleration_vector()
{
	// acceleration vector itself - should be magnified (otherwise it's just too short)
	// and placed at player's position.
	Point p = game->summ_acceleration(game->player().position);  
	Point position = game->player().position - screen_shift;
	sf::Vertex line[] = { sf::Vertex(), sf::Vertex() };
	line[0].position = position.vector();
	line[0].color = sf::Color::White;
	position += p * 1000;    // <-- this is an arbitrary magnifier
	line[1].position = position.vector();
	line[1].color = sf::Color::White;
	window->draw(line, 2, sf::Lines);
}

/////////////////////////////////////////
//          Auxiliary stuff            //
/////////////////////////////////////////

/**------------------------------------------------------------
  Shows distance and running debug output.
  May show other stats as well.
  -----------------------------------------------------------*/
void GameGraphics::show_flyer_stats()
{
	//logger->start("stats::string total");
	std::stringstream ss;
	ss << "distance: " << (int)(game->distance()) 
	   << "\n\n" << message;
	std::string s = ss.str();

	//logger->start("stats::string setting");
	text.setString(/*ss.str()*/s);
	//logger->stop("stats::string setting");
	
	//logger->start("stats::string drawing");
	window->draw(text);
	//logger->stop("stats::string drawing");
	//logger->stop("stats::string total");
}

/**------------------------------------------------------------
  Draws points showing where are invisible stars, if they are
  close enough to window borders.
  -----------------------------------------------------------*/
void GameGraphics::draw_star_indicators()
{
	for (int i = 0; i < game->n_stars(); i++)
	{
		Point star_position = game->star(i).position - screen_shift;
		if (
			to_be_indicated(star_position.x, config->WIDTH, star_position.y, config->HEIGHT)
			|| to_be_indicated(star_position.y, config->HEIGHT, star_position.x, config->WIDTH)
			)
		{ 
			Point coords = calculate_indicator_position(star_position); 
			sf::CircleShape indicator(3);
			indicator.setFillColor(sf::Color::Red);
			indicator.setOrigin(3, 3);
			indicator.setPosition(coords.x, coords.y);
			window->draw(indicator);
		}
	}
}

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
	background->background_shift += move;
}

/**------------------------------------------------------------
  Measures if invisible star should be indicated.
  Star center should be beyond screen, but not too far.
  Method is called twice - for horisontal and vertical coords.
  -----------------------------------------------------------*/
bool GameGraphics::to_be_indicated(double first_coord, double first_measure, 
								   double second_coord, double second_measure) const
{
	return abs(first_coord) > (first_measure / 2)
		   && abs(first_coord) < (first_measure / 2 * config->INDICATOR_COEFF)
		   && abs(second_coord) <= (second_measure / 2 + config->INDICATOR_MARGIN);
}

/**------------------------------------------------------------
  This is a simple proportion: both coordinates should be
  within screen and one should be at "indicator frame" - at
  INDICATOR_MARGIN distance from screen borders.
  -----------------------------------------------------------*/
Point GameGraphics::calculate_indicator_position(const Point& screen_coords) const
{
	// if coordinate is in bounds, coefficient will be more than 1.
	// We pick the smallest.
	float coeff_x = (config->WIDTH / 2 - config->INDICATOR_MARGIN) / abs(screen_coords.x);
	float coeff_y = (config->HEIGHT / 2 - config->INDICATOR_MARGIN) / abs(screen_coords.y);

	return screen_coords * ((coeff_x < coeff_y) ? coeff_x : coeff_y);
}

GameGraphics* gamegraphics = nullptr;