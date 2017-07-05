#include "InterimScreen.h"

#include <string>

#include "GameGraphics.h"
#include "Game.h"

using std::string;

extern GameGraphics* gamegraphics;
extern Game* game;

InterimScreen::InterimScreen(void)
	: window(gamegraphics->window)
{
}


InterimScreen::~InterimScreen(void)
{
}

void InterimScreen::show(int dist)
{
	window.clear(sf::Color(44, 20, 53, 255));
	showMenu(dist);  // <-- also STUB
	window.display();
}


void InterimScreen::showMenu(int dist)
{
	string menutext = "space - start a new game\n\n"
		              "    d     - set difficulty\n\n"
					  "    s     - set screen size\n\n"
					  "    b     - fly bot for me\n\n"
					  "    ?     - about game\n\n"
					  "    q     - quit";
	sf::Text text;
	text.setFont(gamegraphics->font);
	text.setCharacterSize(42);
//	text.setColor(sf::Color(255, 230, 255, 255));
	if (dist == -1) { text.setString("WELCOME!"); }
	else            { text.setString("Distance: " + std::to_string(dist)); }
	text.setOrigin(text.getGlobalBounds().width / 2, text.getGlobalBounds().height / 2);
	text.setPosition(window.getSize().x / 2, window.getSize().y / 2 - 250);
	window.draw(text);

	text.setString(menutext);
	text.setOrigin(text.getGlobalBounds().width / 2, text.getGlobalBounds().height / 2);
	text.setPosition(window.getSize().x / 2, window.getSize().y / 2 + 50);
	window.draw(text);
}