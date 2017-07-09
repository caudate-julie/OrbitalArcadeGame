#include "main.h"
#include <random>
#include <ctime>
#include <SFML\Graphics.hpp>

#include "GameGraphics.h"
#include "Game.h"
#include "Configuration.h"
#include "Background.h"
#include "TimedEvent.h"
#include "Logger.h"

extern Configuration* config;
extern GameGraphics* gamegraphics;
extern Background* background;
extern Game* game;
extern Logger* logger;

sf::RenderWindow* window;   // main and unique! window

/**------------------------------------------------------------
  Menu menuscreen.
  Calls for MenuScreen class and handles all user keyboard 
  interactions.
  Returns true - for new game to start,
          false - to quit the game.
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
			if (event.type == sf::Event::Closed) { window->close(); }

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

void pre_gameloop(vector<unique_ptr<TimedEvent>>* timers)
{
	game->reset();
	background->reset();
	gamegraphics->reset();   // gamegraphics depends on game to create initial set of stars

	timers->push_back(unique_ptr<TimedEvent>(new TimedGameMove()));
	timers->push_back(unique_ptr<TimedEvent>(new TimedStarRevision()));
	timers->push_back(unique_ptr<TimedEvent>(new TimedBotAction()));
	// logger->log("Pre-game resets done");
}


/**------------------------------------------------------------
  Main game loop. Handles:
  * reset of all global variables in the beginning (!time-consuming!)
  * calls to redraw screen (every loop, though it's overkill now)
  * user keyboard interactions (controls & game exit)
  * calls for timed events (moving flyers first of all)
  -----------------------------------------------------------*/
void gameloop()
{
	//logger->log(">> waiting for user choice << ");
	vector<unique_ptr<TimedEvent>> timers;
	pre_gameloop(&timers);

	sf::Clock mainclock;
	game->start();           // all autobots start to calculate paths
	//logger->log("Game started");
	//logger->log("--------------------------------------------");
	int count = 0;
	while (window->isOpen())
	{
		logger->start("one game loop");
		count++;
		sf::Event event;
		while(window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed) { window->close(); }

			if (event.type == sf::Event::KeyPressed)
			{
			//	//logger->start("keyboard processing");
				if (event.key.code == sf::Keyboard::Escape) { return; }
				if (event.key.code == sf::Keyboard::Left) { game->user_turn_on_engine('L'); }
				if (event.key.code == sf::Keyboard::Right) { game->user_turn_on_engine('R'); }
				if (event.key.code == sf::Keyboard::Up) { game->user_turn_on_engine('F'); }
				if (event.key.code == sf::Keyboard::Down) { game->user_turn_on_engine('B'); }
				//logger->stop("keyboard processing");
			}
		}

		// check all timed events.
		//logger->start("timed events");
		int time_elapsed = mainclock.getElapsedTime().asMicroseconds();
		for (int i = 0; i < timers.size(); i++) { timers[i]->on_time(time_elapsed); }
		//logger->stop("timed events");

		//logger->start("logger output");
		gamegraphics->show_message(logger->output());
		//logger->stop("logger output");

		//logger->start("screen redrawn");
		gamegraphics->redraw_game_screen();
		//logger->stop("screen redrawn");

		// STUB! to see the crash of mainflyer. Should be animated crash instead.
		if (game->game_over) 
		{ 
			sf::Clock pause;
			while (pause.getElapsedTime().asSeconds() < 1) {}
			return; 
		}
		logger->stop("one game loop");
	}
}

/**------------------------------------------------------------
  Global window settings (set in the beginning and every time
  window settings are changed).
  -----------------------------------------------------------*/
void set_global_window()
{
	delete window;
	window = new sf::RenderWindow(
		sf::VideoMode(config->WIDTH, config->HEIGHT), 
		"Orbital"
		);
	//window->setFramerateLimit(60);
	window->setMouseCursorVisible(false);

	// (0, 0) screen coordinate is in the middle of the screen.
	sf::View centerview(window->getDefaultView());
	centerview.move(
		static_cast<float>(-config->WIDTH / 2), 
		static_cast<float>(-config->HEIGHT / 2)
		);
	window->setView(centerview);
}

/**------------------------------------------------------------
  Entry point: show start screen, start game, show end screen.
  Generates all global entities.
  -----------------------------------------------------------*/
int WinMain(int argc, char* argv[])
{
	srand((unsigned)time(NULL));   // for Game and GameGraphics.
	
	logger = new Logger();
	//logger->log("");
	config = new Configuration();
	//logger->log("Configuration read");
	gamegraphics =new GameGraphics();
	//logger->log("GameGraphics created");
	game = new Game();
	//logger->log("Game created");
	background = new Background();
	//logger->log("Back created");
	set_global_window();
	//logger->log("Window created");
	//logger->log("-----------------------------------------");

	while (menuscreen()) { gameloop(); };
	
	delete background;
	delete gamegraphics;
	delete game;
	delete window;
	delete config;
	delete logger;
}