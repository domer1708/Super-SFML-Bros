#include "Enemy.h"

Enemy::Enemy(float startX, float startY)
{
    position = sf::Vector2f(startX, startY);
    speed = 120.f; // Prędkość różowego potworka
    direction = 1; // Na start idzie w prawo

    shape.setFillColor(sf::Color::Magenta);  // Różowy kolor
    shape.setSize(sf::Vector2f(40.f, 40.f)); // Trochę mniejszy niż gracz (50x50)
    shape.setPosition(position);
}

void Enemy::update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms)
{
    // Prosty ruch w lewo lub prawo
    velocity.x = speed * direction;
    position.x += velocity.x * dt.asSeconds();
    shape.setPosition(position);

    // Sprawdzanie kolizji z platformami - jeśli dotknie klocka, to zawraca
    for (const auto& platform : platforms)
    {
        if (shape.getGlobalBounds().intersects(platform.getGlobalBounds()))
        {
            if (direction > 0) // Szedł w prawo -> odbija w lewo
            {
                position.x = platform.getGlobalBounds().left - shape.getGlobalBounds().width;
                direction = -1;
            }
            else if (direction < 0) // Szedł w lewo -> odbija w prawo
            {
                position.x = platform.getGlobalBounds().left + platform.getGlobalBounds().width;
                direction = 1;
            }
            shape.setPosition(position);
        }
    }
}

void Enemy::render(sf::RenderWindow& window)
{
    window.draw(shape);
}

sf::FloatRect Enemy::getGlobalBounds() const
{
    return shape.getGlobalBounds();
}