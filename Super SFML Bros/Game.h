#pragma once

#include "Entity.h"

class Game
{
private:
	unique_ptr<Player> player;
	sf::RenderWindow window;

	void processEvents();
	void update(sf::Time dt);
	void render();

public:
	Game();
	void run();
};