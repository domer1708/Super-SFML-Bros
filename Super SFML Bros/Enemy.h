#pragma once

#include "Entity.h"

class Enemy : public Entity
{
private:
    sf::RectangleShape shape;
    float speed;
    int direction; // 1 = prawo, -1 = lewo

public:
    Enemy(float startX, float startY);

    void update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms) override;
    void render(sf::RenderWindow& window) override;
    sf::FloatRect getGlobalBounds() const;
};