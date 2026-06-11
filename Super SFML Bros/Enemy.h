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

class Boss : public Entity {
private:
    sf::RectangleShape shape;
    int hp;
    float speed;
    bool alive;
    float timer;
    int state; // 0 = stoi, 1 = szarżuje
public:
    Boss(float startX, float startY);
    void update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms) override {} // Puste, używamy updateBoss
    void updateBoss(sf::Time dt, const std::vector<sf::RectangleShape>& platforms, sf::Vector2f playerPos);
    void render(sf::RenderWindow& window) override;
    sf::FloatRect getGlobalBounds() const;
    bool isAlive() const { return alive; }
    int getHp() const { return hp; }
    void takeDamage() { hp--; if (hp <= 0) alive = false; }
    void setHp(int newHp) { hp = newHp; }
};