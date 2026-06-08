#include "Elements.h"

Star::Star(float x, float y)
{
    shape.setRadius(15.f);
    shape.setFillColor(sf::Color::Yellow);
    shape.setPosition(x + 10.f, y + 10.f);
    collected = false;
}

void Star::render(sf::RenderWindow& window)
{
    if (!collected)
    {
        window.draw(shape);
    }
}

sf::FloatRect Star::getBounds() const
{
    return shape.getGlobalBounds();
}

void Star::collect()
{
    collected = true;
}

bool Star::isCollected() const
{
    return collected;
}

Trap::Trap(float x, float y)
{
    shape.setRadius(20.f);
    shape.setPointCount(3);
    shape.setFillColor(sf::Color(150, 150, 150));
    shape.setPosition(x + 5.f, y + 10.f);
    damage = 1;
}

void Trap::render(sf::RenderWindow& window)
{
    window.draw(shape);
}

sf::FloatRect Trap::getBounds() const
{
    return shape.getGlobalBounds();
}

int Trap::getDamage() const
{
    return damage;
}