#pragma once

#include <SFML/Graphics.hpp>

// --- KLASA GWIAZDKI (ZNAJDŹKI) ---
class Star
{
private:
    sf::ConvexShape shape;
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

class Portal
{
private:
    sf::RectangleShape shape;
    bool locked; // Czy portal wymaga klucza?
public:
    Portal(float x, float y, bool isLocked = false);
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    void unlock();
    bool isLocked() const;
};

// --- KLASA GRZYBKA SUPER MOCY ---
class Mushroom
{
private:
    sf::Sprite sprite;
    bool collected;

public:
    Mushroom(float x, float y, const sf::Texture& tex);
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    void collect();
    bool isCollected() const;
};

// --- KLASA TRAMPOLINY ---
class Trampoline {
private:
    sf::RectangleShape shape;
public:
    Trampoline(float x, float y);
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
};

// --- KLASA KLUCZA ---
class Key {
private:
    sf::RectangleShape shape;
    bool collected;
public:
    Key(float x, float y);
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    void collect();
    bool isCollected() const;
};

// --- CHECKPOINT ---
class Checkpoint {
private:
    sf::RectangleShape shape;
    bool activated;
public:
    Checkpoint(float x, float y);
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    void activate();
    bool isActivated() const { return activated; }
};

// --- ZANIKAJĄCA PLATFORMA ---
class VanishingPlatform {
private:
    sf::RectangleShape shape;
    float timer;
    enum State { Normal, Triggered, Vanished } state;
public:
    VanishingPlatform(float x, float y);
    void update(sf::Time dt);
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    sf::RectangleShape getShape() const { return shape; }
    void trigger();
    bool isSolid() const { return state != Vanished; }
};

// --- POCISK WIEŻYCZKI ---
class Bullet {
private:
    sf::RectangleShape shape;
    float speed;
    bool alive;
public:
    Bullet(float x, float y);
    void update(sf::Time dt);
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    bool isAlive() const { return alive; }
    void destroy() { alive = false; }
};

// --- STRZELAJĄCA WIEŻYCZKA ---
class Turret {
private:
    sf::RectangleShape shape;
    float shootTimer;
    float shootInterval;
public:
    Turret(float x, float y);
    bool updateAndCheckShoot(sf::Time dt); // Zwraca true, gdy nadszedł czas na strzał
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
};

// --- RUCHOMA PLATFORMA ---
class MovingPlatform {
private:
    sf::RectangleShape shape;
    sf::Vector2f startPos;
    sf::Vector2f deltaMovement; // O ile przesunęła się w tej klatce (kluczowe dla gracza!)
    float timeAccumulator;
    float speed;
    float range;
public:
    MovingPlatform(float x, float y);
    void update(sf::Time dt);
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    sf::RectangleShape getShape() const { return shape; }
    sf::Vector2f getDeltaMovement() const { return deltaMovement; }
};