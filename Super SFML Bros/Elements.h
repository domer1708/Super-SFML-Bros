#pragma once

#include <SFML/Graphics.hpp>

// --- KLASA GWIAZDKI (ZNAJDŹKI) ---
class Star
{
private:
    sf::CircleShape shape;
    bool collected; // Czy została zebrana?

public:
    Star(float x, float y);

    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;

    void collect();
    bool isCollected() const;
};

// --- KLASA PUŁAPKI (KOLCÓW) ---
class Trap
{
private:
    sf::CircleShape shape;
    int damage;

public:
    Trap(float x, float y);

    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    int getDamage() const;
};