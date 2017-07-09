#pragma once

#include <SFML\Graphics.hpp>

class MenuScreen
{
public:
	MenuScreen(void);
	~MenuScreen(void);

	void show(int dist);  // <-- STUB - must be Achievement class

private:
	void showMenu(int dist);
};

