#include "main.h"
#include <random>
#include <ctime>
#include <SFML\Graphics.hpp>

#include "GameGraphics.h"
#include "Game.h"
#include "Configuration.h"
#include "Background.h"
#include "TimedEvent.h"

extern Configuration* config;
extern GameGraphics* gamegraphics;
extern Background* background;
extern Game* game;

sf::RenderWindow* window;

/**------------------------------------------------------------
  Main game loop.  
  -----------------------------------------------------------*/

bool menuscreen()
{
	int dist = game->distance();

	gamegraphics->show_start_screen();
	while(window->isOpen())
	{
		sf::Event event;
		while(window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window->close();
			}
			if (event.type == sf::Event::KeyPressed)
			{
				switch(event.key.code)
				{
				case sf::Keyboard::Space:
					config->set_bot_player(false);
					return true;
				case sf::Keyboard::Escape:
					window->close();
					return false;
				case sf::Keyboard::B:
					config->set_bot_player(true);
					gamegraphics->show_message("ESC to stop");
					return true;
				}
			}
		}
	}
	return false;
}

/**------------------------------------------------------------
  Main game loop.  
  -----------------------------------------------------------*/
void gameloop()
{
	game->reset();
	background->reset();
	gamegraphics->reset();   // gamegraphics depends on game to create initial set of stars

	vector<unique_ptr<TimedEvent>> timers;
	timers.push_back(unique_ptr<TimedEvent>(new TimedGameMove()));
	timers.push_back(unique_ptr<TimedEvent>(new TimedStarRevision()));
	timers.push_back(unique_ptr<TimedEvent>(new TimedBotAction()));

	sf::Clock clock;
	game->start();
	while (window->isOpen())
	{
		sf::Event event;
		while(window->pollEvent(event))
		{
			// get all interactive (mouse, keyboard) events.
			if (event.type == sf::Event::Closed)
			{
				window->close();
			}

			// keyboard.
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Left) { game->user_turn_on_engine('L'); }
				if (event.key.code == sf::Keyboard::Right) { game->user_turn_on_engine('R'); }
				if (event.key.code == sf::Keyboard::Escape) { return; }
			}
		}

		// get all timed events.
		int time_elapsed = clock.getElapsedTime().asMicroseconds();
		for (int i = 0; i < timers.size(); i++) { timers[i]->on_time(time_elapsed); }

		gamegraphics->redraw_game_screen();

		if (game->game_over) 
		{ 
			sf::Clock pause;
			while (pause.getElapsedTime().asSeconds() < 3) {}
			return; 
		}
	}
}

/**------------------------------------------------------------
  Global window settings (set in the beginning and every time
  window settings are changed).
  -----------------------------------------------------------*/
void set_global_window()
{
	window = new sf::RenderWindow(sf::VideoMode(config->WIDTH, config->HEIGHT), "Orbital");
	window->setFramerateLimit(60);
	window->setMouseCursorVisible(false);
	sf::View centerview(window->getDefaultView());
	centerview.move(-config->WIDTH / 2, -config->HEIGHT / 2);
	window->setView(centerview);
}

/**------------------------------------------------------------
  Entry point: show start screen, start game, show end screen.
  -----------------------------------------------------------*/
int WinMain(int argc, char* argv[])
{

	srand((unsigned)time(NULL));   // for Game and GameGraphics.
	
	config = new Configuration();
	gamegraphics =new GameGraphics();
	game = new Game();
	background = new Background();
	set_global_window();

	while (menuscreen()) { gameloop(); };
	
	delete game;
	delete gamegraphics;
	delete background;
	delete window;
	delete config;
}