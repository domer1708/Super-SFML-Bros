#include "Enemy.h"

Enemy::Enemy(float startX, float startY)
{
    position = sf::Vector2f(startX, startY);
    speed = 120.f; // Prędkość różowego potworka
    direction = 1; // Na start idzie w prawo

    shape.setFillColor(sf::Color::Magenta);  // Różowy kolor
    shape.setSize(sf::Vector2f(40.f, 40.f)); // Trochę mniejszy niż gracz (50x50)
    shape.setPosition(position);
    alive = true;
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

// --- BOSS ---
Boss::Boss(float startX, float startY) {
    position = sf::Vector2f(startX, startY);
    hp = 5; // Boss ma 5 żyć
    speed = 250.f;
    alive = true;
    state = 0;
    timer = 0.f;
    shape.setFillColor(sf::Color(139, 0, 0)); // Ciemnoczerwony wielki kloc
    shape.setSize(sf::Vector2f(80.f, 80.f));  // Znacznie większy
    shape.setPosition(position);
}

void Boss::updateBoss(sf::Time dt, const std::vector<sf::RectangleShape>& platforms, sf::Vector2f playerPos) {
    velocity.y += 1000.f * dt.asSeconds(); // Grawitacja

    // --- POPRAWKA 1: BOSS ZASYPIA, GDY JESTEŚ DALEKO ---
    float distanceToPlayer = std::abs(playerPos.x - position.x);
    if (distanceToPlayer < 800.f) { // Budzi się, gdy gracz jest na tym samym ekranie
        timer += dt.asSeconds();

        // Maszyna Stanów
        if (state == 0) { // Odpoczywa
            velocity.x = 0;
            if (timer > 2.0f) { // Co 2 sekundy atakuje
                state = 1;
                timer = 0.f;
                if (playerPos.x > position.x) velocity.x = speed;
                else velocity.x = -speed;
            }
        }
        else if (state == 1) { // Szarżuje
            if (timer > 1.5f) { // Szarża trwa 1.5 sekundy
                state = 0;
                timer = 0.f;
            }
        }
    }
    else {
        velocity.x = 0.f; // Boss stoi i czeka, nie popełnia samobójstwa!
    }

    // Ruch X i kolizje
    position.x += velocity.x * dt.asSeconds();
    shape.setPosition(position);
    for (const auto& platform : platforms) {
        if (shape.getGlobalBounds().intersects(platform.getGlobalBounds())) {
            if (velocity.x > 0) position.x = platform.getGlobalBounds().left - shape.getGlobalBounds().width;
            else if (velocity.x < 0) position.x = platform.getGlobalBounds().left + platform.getGlobalBounds().width;
            velocity.x = 0.f;
            shape.setPosition(position);
        }
    }

    // Ruch Y i kolizje (podłoga)
    position.y += velocity.y * dt.asSeconds();
    shape.setPosition(position);
    for (const auto& platform : platforms) {
        if (shape.getGlobalBounds().intersects(platform.getGlobalBounds())) {
            // --- POPRAWKA 2: UNIKANIE ZACINANIA SIĘ O KLOCKI (-0.1f) ---
            if (velocity.y > 0) position.y = platform.getGlobalBounds().top - shape.getGlobalBounds().height - 0.1f;
            else if (velocity.y < 0) position.y = platform.getGlobalBounds().top + platform.getGlobalBounds().height;
            velocity.y = 0.f;
            shape.setPosition(position);
        }
    }
}

void Boss::render(sf::RenderWindow& window) { window.draw(shape); }
sf::FloatRect Boss::getGlobalBounds() const { return shape.getGlobalBounds(); }