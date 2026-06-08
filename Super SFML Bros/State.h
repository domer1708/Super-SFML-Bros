#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Entity.h"
#include "Level.h"
#include <fstream>
#include "PauseMenu.h"

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
	sf::RectangleShape characterBoxes[3];
	sf::Text characterNames[3];
	int selectedCharacterIndex;
	struct Star 
	{
        sf::Vector2f position;
        float speed;
        float size;
    };
    std::vector<Star> stars;
    void initStars(); // funkcja pomocnicza do tworzenia gwiazd
    // -----------------------------


public:
	MenuState();

	StateAction handleEvent(sf::Event& event) override;
	StateAction update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
	int getSelectedCharacter() const { return selectedCharacterIndex; }
};

class PlayState : public State
{
private:
    std::unique_ptr<Player> player;
    sf::View camera;
    Level currentLevel;

    int currentLevelNumber;
    int currentCharacterIndex; // Zapamiętujemy kim gramy
    bool isGameWon;
    sf::Text gameWonText;

    bool isGameOver;
    sf::Font font;
    sf::Text gameOverText;
    sf::Text resetText;
    sf::ConvexShape heartShape;

    // Logika pauzy
    std::unique_ptr<PauseMenu> pauseMenu;
    bool isPaused;

    void saveGame();
    void loadGame();

public:
    PlayState(int characterIndex);
    StateAction handleEvent(sf::Event& event) override;
    StateAction update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;
};