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
// ==========================================
// --- BOSS ---
// ==========================================
// ZMIANA: Nowy konstruktor z teksturą!
// ==========================================
// --- BOSS ---
// ==========================================
Boss::Boss(float startX, float startY, const sf::Texture& tex) : hp(100), speed(50.0f), phase(1) {
    sprite.setTexture(tex);

    // =======================================================
    // --- POPRAWIONE SKALOWANIE DLA LEPSZEGO WYGLĄDU ---
    // =======================================================
    // Ustawiamy docelową wysokość na 100px (2 kafelki),
    // a szerokość zwiększamy do 140px (prawie 3 kafelki).
    float targetWidth = 140.f;  // ZWIĘKSZONO z 100.f
    float targetHeight = 100.f; // Pozostawiono bez zmian

    // Pobieramy oryginalny rozmiar grafiki
    float texW = static_cast<float>(tex.getSize().x);
    float texH = static_cast<float>(tex.getSize().y);

    // Obliczamy nowe proporcje skalowania
    sprite.setScale(targetWidth / texW, targetHeight / texH);

    // Pozycjonowanie z poprzedniej poprawki (stoi NA platformie)
    sprite.setPosition(startX, startY - 50.f);
}

void Boss::updateBoss(float deltaTime, const std::vector<sf::RectangleShape>& platforms, sf::Vector2f playerPos) {
    if (hp <= 50 && phase == 1) {
        phase = 2;
        speed *= 1.5f; // Boss przyspiesza w drugiej fazie
        sprite.setColor(sf::Color(255, 100, 100)); // Robi się lekko czerwony
    }

    // Prosty ruch: boss idzie w stronę gracza
    if (playerPos.x < sprite.getPosition().x) {
        sprite.move(-speed * deltaTime, 0.0f);
    }
    else {
        sprite.move(speed * deltaTime, 0.0f);
    }
}

void Boss::render(sf::RenderTarget& target) { target.draw(sprite); }
void Boss::takeDamage() { hp -= 25; } // Boss pada na 4 hity
bool Boss::isAlive() const { return hp > 0; }
sf::FloatRect Boss::getGlobalBounds() const { return sprite.getGlobalBounds(); }
sf::Vector2f Boss::getPosition() const { return sprite.getPosition(); }