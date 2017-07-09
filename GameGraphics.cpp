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

/**------------------------------------------------------------
  Creates new GameGraphics, sets up the window->
  -----------------------------------------------------------*/
GameGraphics::GameGraphics() 
	: foreground_redrawing_thread()
{
	font.loadFromFile("a_nova.ttf");
	text = sf::Text();
	text.setFont(font);
	text.setCharacterSize(14);
	text.setPosition(
		static_cast<float>(-config->WIDTH / 2 + 5), 
		static_cast<float>(-config->HEIGHT / 2 + 5)
		);

	show_acceleration_vector = true;

	flyershape = sf::CircleShape(6, 3);
//	flyershape.setScale(0.8f, 1.f);
	flyershape.setOrigin(6, 6);
}

/**------------------------------------------------------------
  Destructor =)
  -----------------------------------------------------------*/
GameGraphics::~GameGraphics() 
{
	foreground_stop = true;
	if (foreground_redrawing_thread.joinable()) { foreground_redrawing_thread.join(); }
	delete foreground;
}


void GameGraphics::reset()
{
	screen_shift = Point(0, 0);
	message = "";

	while(!graphics_queue.empty()) { graphics_queue.pop(); }

	foreground_stop = true;
	if (foreground_redrawing_thread.joinable()) { foreground_redrawing_thread.join(); }
	foreground_stop = false;
	foreground_redrawing_thread.swap(std::thread(&GameGraphics::reset_foreground, this));
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
	logger->start("window cleared");
	window->clear(sf::Color::Black);
	logger->stop("window cleared");

	update_screen_shift();

	logger->start("background drawing");
	background->draw();
	logger->stop("background drawing");

	logger->start("stars drawing");
	draw_stars(); 
	logger->stop("stars drawing");

	logger->start("flyers drawing");
	for (int i = 0; i < game->n_bots(); i++) { draw_flyer(game->bot(i), 'B'); }
	draw_flyer(game->player(), 'M');
	show_flyer_stats();
	if (show_acceleration_vector) { draw_acceleration_vector(); }
	logger->stop("flyers drawing");

	logger->start("window displayed");
	window->display();
	logger->stop("window displayed");
}

void GameGraphics::reset_foreground()
{
	while (!foreground_stop)
	{
		sf::RenderTexture* new_foreground = new sf::RenderTexture();
		Point new_shift = screen_shift;
		new_foreground->create(config->WIDTH * 3, config->HEIGHT * 3);
		new_foreground->clear(sf::Color::Transparent);
	
		sf::View fv (new_foreground->getView());
		fv.move(
			static_cast<float>(-config->WIDTH * 1.5), 
			static_cast<float>(-config->HEIGHT * 1.5)
			);
		new_foreground->setView(fv);

		for (int i = 0; i < game->n_stars(); i++)
		{
			// right now stars are back to circles - that makes them in place
			sf::CircleShape star_shadow(static_cast<float>(game->star(i).size));
			star_shadow.setOrigin(
				static_cast<float>(game->star(i).size), 
				static_cast<float>(game->star(i).size)
				);
			star_shadow.setFillColor(sf::Color(100, 100, 0));
			star_shadow.setPosition(
				static_cast<float>(game->star(i).position.x - new_shift.x),
				static_cast<float>(game->star(i).position.y - new_shift.y)
				);
			new_foreground->draw(star_shadow);
			// ...and no need to set them right until we start to really generate them
		}
		new_foreground->display();

		sf::Sprite* new_sprite = new sf::Sprite(new_foreground->getTexture());

		std::lock_guard<std::mutex> lock_foreground_refreshing(foreground_mutex);
		foreground_shift = new_shift;
		delete foreground;
		delete foreground_sprite;
		foreground = new_foreground;
		foreground_sprite = new_sprite;
	}
}

/**------------------------------------------------------------
  End screen. Probably should be in separate class -
  too many elements.
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
	if (!foreground || !foreground_sprite) { return; }

	logger->start("mutex set");
	foreground_mutex.lock();
	logger->stop("mutex set");

	Point current_shift = foreground_shift - screen_shift;

	logger->start("sprite drawing");
	foreground_sprite->setOrigin(
		static_cast<float>(foreground_sprite->getGlobalBounds().width / 2 - current_shift.x), 
		static_cast<float>(foreground_sprite->getGlobalBounds().height / 2 - current_shift.y)
		);
	
	window->draw(*foreground_sprite);
	logger->stop("sprite drawing");
	foreground_mutex.unlock();

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


GameGraphics* gamegraphics = nullptr;