#pragma once

#include "Entity.h"

enum class StateAction // stany
{
	Keep,
	Menu,
	Play,
	Exit
};

class State // klasa bazowa
{
public:
	virtual ~State() = default;

	virtual StateAction handleEvent(sf::Event& event) = 0;
	virtual StateAction update(sf::Time dt) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
};

class MenuState : public State
{
private:
	sf::Font font;  // czcionka
	sf::Text title; // tytuł
	sf::RectangleShape menuFrame; // ramka
	sf::Text menu[2]; // dwie opcje (graj, wyjdź)
	int selectedItemIndex; // index zaznaczonej opcji

	bool isChoosingCharacter;
	sf::RectangleShape characterBoxes[2];
	sf::Text characterNames[2];
	int selectedCharacterIndex;

public:
	MenuState();

	StateAction handleEvent(sf::Event& event) override;
	StateAction update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};

class PlayState : public State
{
private:
	std::unique_ptr<Player> player; // wskaźnik na gracza
	sf::View camera; // kamera podążająca za graczem
	Level currentLevel;

public:
	PlayState();

	StateAction handleEvent(sf::Event& event) override;
	StateAction update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};