#include "Elements.h"

Star::Star(float x, float y)
{
    shape.setPointCount(10); // Nasza gwiazda ma 10 wierzchołków

    float outerRadius = 15.f; // Długość ramienia
    float innerRadius = 6.f;  // Głębokość wcięcia między ramionami
    const float PI = 3.14159265f;

    // Pętla przelicza i układa 10 punktów na kole
    for (int i = 0; i < 10; i++)
    {
        // Kąt dla kolejnego punktu (co 36 stopni, czyli PI/5 radianów).
        // Odejmujemy PI/2, aby pierwsze ramię gwiazdy wskazywało idealnie w górę.
        float angle = i * (PI / 5.f) - (PI / 2.f);

        // Parzyste punkty (0, 2, 4...) to ramiona, nieparzyste (1, 3, 5...) to wcięcia
        float radius = (i % 2 == 0) ? outerRadius : innerRadius;

        // Trygonometria obliczająca pozycję wierzchołka (X, Y)
        float px = radius * std::cos(angle) + outerRadius;
        float py = radius * std::sin(angle) + outerRadius;

        shape.setPoint(i, sf::Vector2f(px, py));
    }

    shape.setFillColor(sf::Color::Yellow);

    // Ustawienie gwiazdki na mapie (lekko wyśrodkowane w klocku 50x50)
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