#include "Enemy.h"

Enemy::Enemy(float startX, float startY, const sf::Texture& tex)
{
    position = sf::Vector2f(startX, startY);
    speed = 120.f;
    direction = -1; // Na start idzie w lewo

    sprite.setTexture(tex);

    float texW = static_cast<float>(tex.getSize().x);
    float texH = static_cast<float>(tex.getSize().y);

    // ROZSZERZANIE: 60 szerokości, 40 wysokości (wygląda bardziej jak półkole/kopuła)
    float scaleX = 60.f / texW;
    float scaleY = 40.f / texH;
    sprite.setScale(scaleX, scaleY);

    // Hitbox zostawiamy 40x40, by gracz nie ginął od byle muśnięcia boku potwora
    shape.setFillColor(sf::Color::Transparent);
    shape.setSize(sf::Vector2f(40.f, 40.f));

    alive = true;
}

void Enemy::update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms)
{
    velocity.x = speed * direction;
    position.x += velocity.x * dt.asSeconds();

    // Hitbox 40x40 wyśrodkowany w kafelku 50x50 (+5px w X, +10px w Y, by dotykał ziemi)
    shape.setPosition(position.x + 5.f, position.y + 10.f);

    for (const auto& platform : platforms)
    {
        if (shape.getGlobalBounds().intersects(platform.getGlobalBounds()))
        {
            if (direction > 0)
            {
                position.x = platform.getGlobalBounds().left - shape.getGlobalBounds().width - 5.f;
                direction = -1;
            }
            else if (direction < 0)
            {
                position.x = platform.getGlobalBounds().left + platform.getGlobalBounds().width - 5.f;
                direction = 1;
            }
            shape.setPosition(position.x + 5.f, position.y + 10.f);
        }
    }

    // Sprite 60x40 wyśrodkowany w kafelku 50x50 (-5px w X, +10px w Y, by dotykał ziemi)
    sprite.setPosition(position.x - 5.f, position.y + 10.f);

    float texW = static_cast<float>(sprite.getTexture()->getSize().x);
    float texH = static_cast<float>(sprite.getTexture()->getSize().y);

    // Zachowanie nowych proporcji (60x40) przy obrocie
    if (direction < 0) {
        sprite.setScale(60.f / texW, 40.f / texH);
        sprite.setOrigin(0.f, 0.f);
    }
    else {
        sprite.setScale(-60.f / texW, 40.f / texH);
        sprite.setOrigin(texW, 0.f);
    }
}

void Enemy::render(sf::RenderWindow& window)
{
    window.draw(sprite);
}

sf::FloatRect Enemy::getGlobalBounds() const
{
    return shape.getGlobalBounds();
}

// ==========================================
// --- BOSS ---
// ==========================================
Boss::Boss(float startX, float startY) {
    position = sf::Vector2f(startX, startY);
    hp = 5;
    speed = 250.f;
    alive = true;
    state = 0;
    timer = 0.f;
    shape.setFillColor(sf::Color(139, 0, 0));
    shape.setSize(sf::Vector2f(80.f, 80.f));
    shape.setPosition(position);
}

void Boss::updateBoss(sf::Time dt, const std::vector<sf::RectangleShape>& platforms, sf::Vector2f playerPos) {
    velocity.y += 1000.f * dt.asSeconds();

    float distanceToPlayer = std::abs(playerPos.x - position.x);
    if (distanceToPlayer < 800.f) {
        timer += dt.asSeconds();

        if (state == 0) {
            velocity.x = 0;
            if (timer > 2.0f) {
                state = 1;
                timer = 0.f;
                if (playerPos.x > position.x) velocity.x = speed;
                else velocity.x = -speed;
            }
        }
        else if (state == 1) {
            if (timer > 1.5f) {
                state = 0;
                timer = 0.f;
            }
        }
    }
    else {
        velocity.x = 0.f;
    }

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

    position.y += velocity.y * dt.asSeconds();
    shape.setPosition(position);
    for (const auto& platform : platforms) {
        if (shape.getGlobalBounds().intersects(platform.getGlobalBounds())) {
            if (velocity.y > 0) position.y = platform.getGlobalBounds().top - shape.getGlobalBounds().height - 0.1f;
            else if (velocity.y < 0) position.y = platform.getGlobalBounds().top + platform.getGlobalBounds().height;
            velocity.y = 0.f;
            shape.setPosition(position);
        }
    }
}

void Boss::render(sf::RenderWindow& window) { window.draw(shape); }
sf::FloatRect Boss::getGlobalBounds() const { return shape.getGlobalBounds(); }