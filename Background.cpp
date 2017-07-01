#include "Background.h"
#include <random>

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
	this->galaxy.create(conf.WIDTH * 2, conf.HEIGHT * 2);
	this->corner = Point((conf.WIDTH / 2), (conf.HEIGHT / 2));
	int BACK_STAR_N = 20000;
	this->backstars.reserve(BACK_STAR_N);
	this->galaxy.clear();
	for (int i = 0; i < BACK_STAR_N; i++)
	{
		sf::Vertex v;
		Point p(rand() % conf.WIDTH, rand() % conf.HEIGHT);
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
Background::~Background(void) { /*delete this->instance;*/ }

/**------------------------------------------------------------
  
  -----------------------------------------------------------*/
void Background::draw()
{
	sf::Sprite sprite (this->galaxy.getTexture());
	GameGraphics::get().window.draw(sprite);
	GameGraphics::get().window.draw(sprite);
}

/**------------------------------------------------------------
  
  -----------------------------------------------------------*/
void Background::draw_stars()
{
	for (sf::CircleShape s : this->backstars)
	{
		GameGraphics::get().window.draw(s);
	}
}
