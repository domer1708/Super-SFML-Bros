#include "Game.h"
#include <iostream>

Game::Game() : window(sf::VideoMode(800, 600), "Super SFML Bros")
{
	currentState = std::make_unique<MenuState>(); // zaczynamy z menu
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

void Game::handleStateChange(StateAction action)
{
	if (action == StateAction::Play)
	{
		// 1. Wyciągamy informacje z Menu
		MenuState* menu = dynamic_cast<MenuState*>(currentState.get());
		int chosenCharacter = 0; // Domyślnie Mario (0)

		if (menu != nullptr)
		{
			chosenCharacter = menu->getSelectedCharacter();
		}
		// 2. Tworzymy PlayState i podajemy mu indeks z menu
		currentState = std::make_unique<PlayState>(chosenCharacter);
	}
	else if (action == StateAction::Menu)
	{
		currentState = std::make_unique<MenuState>();
	}
	else if (action == StateAction::Exit)
	{
		window.close();
	}
}

void Game::processEvents()
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();
		else
		{
			StateAction action = currentState->handleEvent(event);
			handleStateChange(action);
		}
	}
}

void Game::update(sf::Time dt)
{
	StateAction action = currentState->update(dt);
	handleStateChange(action);
}

void Game::render()
{
	window.clear(sf::Color::Black);
	currentState->render(window);
	window.display();
}