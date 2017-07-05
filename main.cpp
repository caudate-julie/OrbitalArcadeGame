#include "main.h"
#include <random>
#include <ctime>
#include <SFML\Graphics.hpp>
//#include <memory>

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

/**------------------------------------------------------------
  Main game loop.  
  -----------------------------------------------------------*/

bool menuscreen()
{
	int dist = game->distance();

	background->reset();
	game->reset();
	gamegraphics->reset();

	gamegraphics->show_start_screen();
	while(gamegraphics->window.isOpen())
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
				switch(event.key.code)
				{
				case sf::Keyboard::Space:
					return true;
				case sf::Keyboard::Q:
					gamegraphics->window.close();
					return false;
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
	sf::Clock clock;

	vector<unique_ptr<TimedEvent>> timers;
	timers.push_back(unique_ptr<TimedEvent>(new TimedGameMove()));
	timers.push_back(unique_ptr<TimedEvent>(new TimedStarRevision()));
	timers.push_back(unique_ptr<TimedEvent>(new TimedBotAction()));


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

		int time_elapsed = clock.getElapsedTime().asMicroseconds();
		for (int i = 0; i < timers.size(); i++) { timers[i]->on_time(time_elapsed); }

		if (game->crashed()) { 
				return;
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
	gamegraphics =new GameGraphics();

	game = new Game();
	background = new Background();

	while (menuscreen()) 
	{
		gameloop(); 
	};
	

	delete game;
	delete gamegraphics;
	delete background;
	delete config;
	background = nullptr;
	game = nullptr;
	gamegraphics = nullptr;
	config = nullptr;
}