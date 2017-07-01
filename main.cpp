#include "main.h"
#include <random>
#include <ctime>

/**------------------------------------------------------------
  Main game loop.  
  -----------------------------------------------------------*/
void gameloop()
{
	Configuration& conf = Configuration::get();
	Game& game = Game::get();
	GameGraphics& screen = GameGraphics::get();
	sf::Clock game_clock;
	sf::Clock star_revision_clock;
	sf::Clock bot_action_clock;
	game.start();
	while (screen.window.isOpen())
	{
		sf::Event event;
		while(screen.window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				screen.window.close();
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Left) { game.user_turn_on_engine('L'); }
				if (event.key.code == sf::Keyboard::Right) { game.user_turn_on_engine('R'); }
			}
			// обработка событий
		}
		if (game_clock.getElapsedTime() > sf::microseconds(conf.TIME_UNIT))
		{
			game_clock.restart();
			game.make_move();
			if (game.crashed(game.flyer->position, conf.FLYER_SIZE)) { 
				screen.show_end_screen(); 
				return;
			}
			screen.redraw_game_screen();
			screen.show_message(std::to_string(
				(game_clock.getElapsedTime().asMicroseconds() - conf.TIME_UNIT) / 100
				));
		}
		if (star_revision_clock.getElapsedTime() > sf::milliseconds(conf.STAR_REVISE_TIME))
		{
			game.revise_stars();
			star_revision_clock.restart();
		}
		if (bot_action_clock.getElapsedTime() > sf::milliseconds(conf.BOT_ACTION))
		{
			game.call_bots_action();
		}

	}
}

/**------------------------------------------------------------
  Entry point: show start screen, start game, show end screen.
  -----------------------------------------------------------*/
int WinMain(int argc, char* argv[])
{
	srand((unsigned)time(NULL));   // for Game and GameGraphics.
	gameloop();
	return 0;
}