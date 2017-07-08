#include "Background.h"

#include <random>

#include "auxiliary.h"
#include "GameGraphics.h"

extern Configuration* config;
extern sf::RenderWindow* window;

/**------------------------------------------------------------
  
  -----------------------------------------------------------*/
Background::Background(void)
{
	galaxy.create(config->WIDTH * 3, config->HEIGHT * 3);
	
	back_shift = Point(0, 0);
	add_corner = Point((config->WIDTH), (config->HEIGHT));

	int BACK_STAR_N = 50000;
	galaxy.clear(sf::Color::Transparent);
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
		galaxy.draw(&v, 1, sf::Points);
	}
	galaxy.display();
}

/**------------------------------------------------------------
  Destructor.
  -----------------------------------------------------------*/
Background::~Background(void) {  }

void Background::reset()
{
	back_shift = Point(0, 0);
}

/**------------------------------------------------------------
  
  -----------------------------------------------------------*/
void Background::draw()
{
	sf::Sprite sprite(galaxy.getTexture(), sf::IntRect(
												 sf::Vector2<int>(config->WIDTH * 1.5, config->HEIGHT * 1.5)
												 + (sf::Vector2<int>)(back_shift * 0.2).vector(),
												 sf::Vector2<int>(config->WIDTH, config->HEIGHT)
												 ));
	sprite.setOrigin(sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height / 2);
	window->draw(sprite);
}

/**============================================================
  ===================== PRIVATE MEMBERS =======================
  ===========================================================*/


Background* background = nullptr;
