#pragma once

#include <SFML\Graphics.hpp>

class InterimScreen
{
public:
	InterimScreen(void);
	~InterimScreen(void);

	void show(int dist);  // <-- STUB - must be Achievement class

private:
	void showMenu(int dist);
};

