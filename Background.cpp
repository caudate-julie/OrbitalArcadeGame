#include "Background.h"
#include <random>
#include "auxiliary.h"

//Background* Background::instance = nullptr;

/**------------------------------------------------------------
  It's a singleton day!
  -----------------------------------------------------------*/
Background& Background::get()
{
	static Background instance;
	return instance;
}

/**------------------------------------------------------------
  
  -----------------------------------------------------------*/
Background::Background(void)
{
	Configuration& conf = Configuration::get();
	this->galaxy.create(conf.WIDTH * 3, conf.HEIGHT * 3);
	
	this->corner = Point(0, 0);
	this->add_corner = Point((conf.WIDTH), (conf.HEIGHT));

	int BACK_STAR_N = 50000;
//	this->backstars.reserve(BACK_STAR_N);
	this->galaxy.clear();
	for (int i = 0; i < BACK_STAR_N; i++)
	{
		sf::Vertex v;
		Point p(rand() % conf.WIDTH * 3, rand() % conf.HEIGHT * 3);
		int color = pow(rand() % 5 + 2, 3);
		v.color = sf::Color(
			color + rand() % 50,
			color + rand() % 50,
			color + rand() % 50,
			pow(rand() % 5 + 2, 3) + 50
			);
		v.position = sf::Vector2f(p.x, p.y);
		this->galaxy.draw(&v, 1, sf::Points);
	}


	this->galaxy.display();

}

/**------------------------------------------------------------
  Destructor.
  -----------------------------------------------------------*/
Background::~Background(void) {  }

/**------------------------------------------------------------
  
  -----------------------------------------------------------*/
void Background::draw()
{
	sf::Sprite sprite (this->galaxy.getTexture(), sf::IntRect(
												 (sf::Vector2<int>)(corner * 0.2).vector(),
												 sf::Vector2<int>(800, 600)
												 ));
	GameGraphics::get().window.draw(sprite);
	GameGraphics::get().window.draw(sprite);
}

/**------------------------------------------------------------
  
  -----------------------------------------------------------*/
//void Background::draw_stars()
//{
//	for (sf::CircleShape s : this->backstars)
//	{
//		GameGraphics::get().window.draw(s);
//	}
//}
