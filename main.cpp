#include "main.h"
#include <random>
#include <ctime>*/
#include <SFML\Graphics.hpp>
//#include <memory>

extern Configuration* config;
extern GameGraphics* gamegraphics;
extern Background* background;
extern Game* game;

/**------------------------------------------------------------
  Main game loop.  
  -----------------------------------------------------------*/
void gameloop()
{
	sf::Clock game_clock;
	sf::Clock star_revision_clock;
	sf::Clock bot_action_clock;
	game->start();
	while (gamegraphics->window.isOpen())
	{
		sf::Event event;
		while(gamegraphics->window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				gamegraphics->window.close();
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Left) { game->user_turn_on_engine('L'); }
				if (event.key.code == sf::Keyboard::Right) { game->user_turn_on_engine('R'); }
			}
			// обработка событий
		}
		if (game_clock.getElapsedTime() > sf::microseconds(config->TIME_UNIT))
		{
			game_clock.restart();
			game->make_move();
			if (game->crashed()) { 
				gamegraphics->show_end_screen(); 
				return;
			}
			gamegraphics->redraw_game_screen();
		}
		if (star_revision_clock.getElapsedTime() > sf::milliseconds(config->STAR_REVISE_TIME))
		{
			game->revise_stars();
			star_revision_clock.restart();
		}
		if (bot_action_clock.getElapsedTime() > sf::milliseconds(config->BOT_ACTION))
		{
			game->call_bots_action();
		}

	}
}

/**------------------------------------------------------------
  Entry point: show start screen, start game, show end screen.
  -----------------------------------------------------------*/
int WinMain(int argc, char* argv[])
{
	srand((unsigned)time(NULL));   // for Game and GameGraphics.
	
	config = new Configuration();
	background = new Background();
	gamegraphics =new GameGraphics();
	game = new Game();

	gameloop();

	delete game;
	delete gamegraphics;
	delete background;
	delete config;
	background = nullptr;
	game = nullptr;
	gamegraphics = nullptr;
	config = nullptr;
}