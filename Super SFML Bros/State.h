#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
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
    Load,
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
    sf::Text menu[3]; // opcje
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

    // TEKSTURY
    sf::Texture mushroomTexture;
    sf::Texture platformTexture;
    sf::Texture backgroundTexture;
    sf::Texture groundTexture;
    sf::Texture doorTexture; // <--- DODANA TEKSTURA DRZWI
    sf::Texture trapTexture;
    sf::Texture keyTexture;
    sf::Texture flagTexture;

    sf::Sprite backgroundSprite;

    // DŹWIĘKI
    sf::SoundBuffer stompBuffer;
    sf::Sound stompSound;
    sf::SoundBuffer powerupBuffer;
    sf::Sound powerupSound;

    bool isGameOver;
    sf::Font font;
    sf::Text gameOverText;
    sf::Text resetText;
    sf::ConvexShape heartShape;
    sf::Text starText;
    sf::Text scoreText;

    // Logika pauzy
    std::unique_ptr<PauseMenu> pauseMenu;

    // Zegar i zgony
    sf::Text timerText;
    sf::Clock gameClock;
    float totalTime;
    bool isPaused;
    sf::Text deathText;
    int deathCount;
    sf::Text keyText;

    void saveGame();
    bool loadGame();
public:
    PlayState(int characterIndex, bool loadFromSave = false);
    StateAction handleEvent(sf::Event& event) override;
    StateAction update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;
};
