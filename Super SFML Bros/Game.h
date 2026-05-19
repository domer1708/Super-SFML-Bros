#pragma once

#include "State.h"

class Game
{
private:
	sf::RenderWindow window; // główne okno
	std::unique_ptr<State> currentState; // wskaźnik na aktualny stan gry

	void processEvents();
	void update(sf::Time dt);
	void render();
	void handleStateChange(StateAction action); // zmiana ekranów

public:
	Game();
	void run();
};