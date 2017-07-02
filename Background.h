#pragma once
#include <SFML\Graphics.hpp>
#include "Point.h"
#include "Configuration.h"
#include "GameGraphics.h"
#include <vector>

class Background
{
public:
	static Background& get();
	~Background(void);

	sf::RenderTexture galaxy;
	Point corner;

	void draw();

private:
	Background(void);

	std::vector<sf::CircleShape> backstars;
	void draw_stars();
};

