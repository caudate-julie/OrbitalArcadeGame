#pragma once
#include <SFML\Graphics.hpp>
//#include <vector>

#include "Point.h"
#include "Configuration.h"

class Background
{
public:
	Background(void);
	~Background(void);

	void reset();

	sf::RenderTexture galaxy;
	sf::Sprite* background_sprite;
	Point background_shift;

	void draw();

private:
	Point add_corner;    // (WIDTH, HEIGHT) from image corner to screen corner

//	std::vector<sf::CircleShape> backstars;
	//void draw_stars();

};