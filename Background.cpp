#include "Background.h"

#include <random>

#include "auxiliary.h"
#include "GameGraphics.h"
#include "Logger.h"

extern Configuration* config;
extern sf::RenderWindow* window;
extern Logger* logger;

/**------------------------------------------------------------
  
  -----------------------------------------------------------*/
Background::Background(void)
{
	primary_galaxy = new sf::RenderTexture(); 
	//secondary_galaxy = new sf::RenderTexture(); 
	primary_galaxy->create(config->WIDTH * 3, config->HEIGHT * 3);
	//secondary_galaxy->create(config->WIDTH * 3, config->HEIGHT * 3);
	
	background_shift = Point(0, 0);
	add_corner = Point((config->WIDTH), (config->HEIGHT));

	int BACK_STAR_N = 50000;
	primary_galaxy->clear(sf::Color::Transparent);
	for (int i = 0; i < BACK_STAR_N; i++)
	{
		sf::Vertex v;
		Point p(rand() % config->WIDTH * 3, rand() % config->HEIGHT * 3);
		int color = static_cast<int>(pow(rand() % 5 + 2, 3));
		v.color = sf::Color(
			color + rand() % 50,
			color + rand() % 50,
			color + rand() % 50,
			static_cast<sf::Uint8>(pow(rand() % 5 + 2, 3) + 50)
			);
		v.position = p.vector();
		primary_galaxy->draw(&v, 1, sf::Points);
	}
	primary_galaxy->display();
}

/**------------------------------------------------------------
  Destructor.
  -----------------------------------------------------------*/
Background::~Background(void) 
{ 
	delete primary_galaxy;
	delete secondary_galaxy;
}

void Background::reset()
{
	background_shift = Point(0, 0);
}

/**------------------------------------------------------------
  
  -----------------------------------------------------------*/
void Background::draw()
{
	//logger->start("sprite drawing");
	sf::Sprite sprite(primary_galaxy->getTexture(), sf::IntRect(
												 sf::Vector2<int>(config->WIDTH * 1.5, config->HEIGHT * 1.5)
												 + (sf::Vector2<int>)(background_shift * 0.2).vector(),
												 sf::Vector2<int>(config->WIDTH, config->HEIGHT)
												 ));
	sprite.setOrigin(sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height / 2);
	window->draw(sprite);
	//logger->stop("sprite drawing");
}

/**============================================================
  ===================== PRIVATE MEMBERS =======================
  ===========================================================*/


Background* background = nullptr;
