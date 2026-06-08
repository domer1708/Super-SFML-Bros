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
		MenuState* menu = dynamic_cast<MenuState*>(currentState.get());
		int chosenCharacter = menu ? menu->getSelectedCharacter() : 0;
		// False = nowa gra
		currentState = std::make_unique<PlayState>(chosenCharacter, false);
	}
	else if (action == StateAction::Load)
	{
		// True = wczytaj z pliku (postać nadpisze się sama z zapisu)
		currentState = std::make_unique<PlayState>(0, true);
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