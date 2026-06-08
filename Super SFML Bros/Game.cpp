#include "Game.h"

Game::Game() : window(sf::VideoMode(800, 600), ":)")
{
	currentState = std::make_unique<MenuState>(); // zaczynamy z menu
}

void Game::run()
{
	sf::Clock clock; // zegar przeliczający klatki

	while (window.isOpen()) // główna pętla
	{
		sf::Time dt = clock.restart();

		processEvents(); // sprawdza wejście
		update(dt); // przelicza fizyke
		render(); // rysuje
	}
}

void Game::handleStateChange(StateAction action)
{
	if (action == StateAction::Play)
    {
        // Sprawdzamy, czy obecny stan to na pewno menu
        MenuState* menu = dynamic_cast<MenuState*>(currentState.get());
        int chosenCharacter = 0; // domyślnie Mario
        
        if (menu != nullptr)
        {
            chosenCharacter = menu->getSelectedCharacter(); // pobieramy wybór z menu
        }

        // Tworzymy nową rozgrywkę, przekazując jej numer wybranej postaci!
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
			StateAction action = currentState->handleEvent(event); // przekazujemy zdarzenie do aktywnego stanu
			handleStateChange(action); // sprawdzenie czy zażądano zmiany
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