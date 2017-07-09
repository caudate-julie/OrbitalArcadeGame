#include "MenuScreen.h"

#include <string>

#include "Game.h"

using std::string;

extern Game* game;
extern sf::RenderWindow* window;

MenuScreen::MenuScreen(void)
{
	
}


MenuScreen::~MenuScreen(void)
{
}

void MenuScreen::show(int dist)
{
	window->clear(sf::Color(44, 20, 53, 255));
	showMenu(dist);  // <-- also STUB
	window->display();
}


void MenuScreen::showMenu(int dist)
{
	string menutext = "space - start a new game\n\n"
		              "    d     - set difficulty\n\n"
					  "    s     - set screen size\n\n"
					  "    b     - fly bot for me\n\n"
					  "    ?     - about game\n\n"
					  "  Esc   - quit";
	sf::Text text;
	sf::Font font;
	font.loadFromFile("a_nova.ttf");
	text.setFont(font);
	text.setCharacterSize(42);
//	text.setColor(sf::Color(255, 230, 255, 255));
	if (dist == -1) { text.setString("WELCOME!"); }
	else            { text.setString("Distance: " + std::to_string(dist)); }
	text.setOrigin(text.getGlobalBounds().width / 2, text.getGlobalBounds().height / 2);
	text.setPosition(0, - 250);
	window->draw(text);

	text.setString(menutext);
	text.setOrigin(text.getGlobalBounds().width / 2, text.getGlobalBounds().height / 2);
	text.setPosition(0, 50);
	window->draw(text);
}