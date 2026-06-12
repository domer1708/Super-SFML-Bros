#pragma once
#include "Entity.h"

class Enemy : public Entity
{
private:
    sf::RectangleShape shape; // Niewidzialny hitbox
    sf::Sprite sprite;        // Nasza grafika potwora
    float speed;
    int direction;
    bool alive;
public:
    // Dodano teksturę do konstruktora
    Enemy(float startX, float startY, const sf::Texture& tex);

    void update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms) override;
    void render(sf::RenderWindow& window) override;
    sf::FloatRect getGlobalBounds() const;
    bool isAlive() const { return alive; }
    void die() { alive = false; }
};

// ==========================================
// --- BOSS ---
// ==========================================
class Boss {
private:
    sf::Sprite sprite;
    int hp;
    float speed;
    int phase; // Fazy bossa (np. 1 - normalnie, 2 - wściekły)

public:
    // ZMIANA: Konstruktor teraz wymaga tekstury!
    Boss(float startX, float startY, const sf::Texture& tex);

    // Główne metody logiki i rysowania
    // ZMIANA: Ujednoliciłem nazwę metody update tak, jak jej używasz w PlayState
    void updateBoss(float deltaTime, const std::vector<sf::RectangleShape>& platforms, sf::Vector2f playerPos);
    void render(sf::RenderTarget& target);

    // Interakcje
    void takeDamage(); // bez argumentu, dopasowane do Twojej logiki
    bool isAlive() const;

    // Gettery (np. do kolizji i zapisu)
    sf::FloatRect getGlobalBounds() const;
    sf::Vector2f getPosition() const;
    int getHp() const { return hp; } // Zwraca aktualne HP do zapisu
    void setHp(int newHp) { hp = newHp; } // Ustawia HP przy wczytywaniu
};