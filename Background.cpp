#include "Background.h"
#include <random>
#include "auxiliary.h"
//#include <memory>

extern Configuration* config;
extern GameGraphics* gamegraphics;
//Background* Background::instance = nullptr;

/**------------------------------------------------------------
  It's a singleton day!
  -----------------------------------------------------------*/
//Background& Background::get()
//{
//	static Background instance;
//	return instance;
//}

/**------------------------------------------------------------
  
  -----------------------------------------------------------*/
Background::Background(void)
{
	//Configuration& conf = Configuration::get();
	galaxy.create(config->WIDTH * 3, config->HEIGHT * 3);
	//galaxy.create(50, 50);
	
	corner = Point(0, 0);
	add_corner = Point((config->WIDTH), (config->HEIGHT));

	int BACK_STAR_N = 50000;
//	backstars.reserve(BACK_STAR_N);
	galaxy.clear();
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

/**------------------------------------------------------------
  
  -----------------------------------------------------------*/
void Background::draw()
{
	sf::Sprite sprite (galaxy.getTexture(), sf::IntRect(
												 (sf::Vector2<int>)(corner * 0.2).vector(),
												 sf::Vector2<int>(800, 600)
												 ));
	/*GameGraphics::get().*/gamegraphics->window.draw(sprite);
	/*GameGraphics::get().*/gamegraphics->window.draw(sprite);
}

/**------------------------------------------------------------
  
  -----------------------------------------------------------*/
//void Background::draw_stars()
//{
//	for (sf::CircleShape s : backstars)
//	{
//		GameGraphics::get().window.draw(s);
//	}
//}

/**============================================================
  ===================== PRIVATE MEMBERS =======================
  ===========================================================*/



Background* background = nullptr;
