#include "Game.h"

Game::Game() : window(sf::VideoMode(800, 600), ":)")
{
	player = make_unique<Player>();
}

void Game::run()
{
	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Time dt = clock.restart();

		processEvents();
		update(dt);
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
		}

		player->handleEvent(event);
	}
}

void Game::update(sf::Time dt)
{
	player->update(dt);
}

void Game::render()
{
	window.clear(sf::Color::Black);
	player->render(window);
	window.display();
}