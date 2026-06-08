#pragma once
#include "Enemy.h"
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
    std::unique_ptr<Player> player; // wskaźnik na gracza
    sf::View camera; // kamera podążająca za graczem
    Level currentLevel;

    // --- NOWE ELEMETY DLA POTWORKÓW I GAME OVER ---
    std::vector<Enemy> enemies; // lista naszych 4 potworków
    
    bool isGameOver;      // czy gracz przegrał?
    sf::Font font;        // czcionka do napisu Game Over
    sf::Text gameOverText;// napis "GAME OVER"
    sf::Text resetText;   // napis pod spodem "Wcisnij ESC, aby wrocic do menu"
    // ----------------------------------------------

public:
    PlayState(int characterIndex);

    StateAction handleEvent(sf::Event& event) override;
    StateAction update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;
};