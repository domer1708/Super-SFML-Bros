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
// --- POCISK BOSSA ---
// ==========================================
BossBullet::BossBullet(float x, float y, sf::Vector2f targetPos, const sf::Texture& tex) {
    sprite.setTexture(tex);

    // Skalowanie kuli (zakładamy rozmiar ok. 30x30 pikseli)
    float texW = static_cast<float>(tex.getSize().x);
    float texH = static_cast<float>(tex.getSize().y);
    sprite.setOrigin(texW / 2.f, texH / 2.f); // Środek obrotu pocisku

    float scaleX = 30.f / texW;
    float scaleY = 30.f / texH;
    sprite.setScale(scaleX, scaleY);
    sprite.setPosition(x, y);

    alive = true;
    lifetime = 4.0f;

    // Trygonometria - wyliczamy wektor lotu prosto w gracza
    sf::Vector2f direction = targetPos - sf::Vector2f(x, y);
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0) direction /= length;

    velocity = direction * 300.f;
}

void BossBullet::update(float dt) {
    sprite.move(velocity * dt);
    // Możesz tutaj dodać obrót lecącej kuli!
    sprite.rotate(360.f * dt); // Kręci się podczas lotu

    lifetime -= dt;
    if (lifetime <= 0.f) alive = false;
}

void BossBullet::render(sf::RenderTarget& target) { if (alive) target.draw(sprite); }
sf::FloatRect BossBullet::getBounds() const { return sprite.getGlobalBounds(); }

// ==========================================
// --- BOSS ---
// ==========================================
Boss::Boss(float startX, float startY, const sf::Texture& tex, const sf::Texture& bulletTex)
    : hp(100), speed(50.0f), phase(1), state(0), stateTimer(0.f), shootTimer(0.f) {

    bulletTexture = &bulletTex; // Zapisujemy adres tekstury kuli, żeby boss wiedział z czego strzelać
    sprite.setTexture(tex);

    float targetWidth = 140.f;
    float targetHeight = 100.f;
    float texW = static_cast<float>(tex.getSize().x);
    float texH = static_cast<float>(tex.getSize().y);
    sprite.setScale(targetWidth / texW, targetHeight / texH);
    sprite.setPosition(startX, startY - 50.f);
}

void Boss::updateBoss(float deltaTime, const std::vector<sf::RectangleShape>& platforms, sf::Vector2f playerPos) {
    if (hp <= 50 && phase == 1) {
        phase = 2;
        speed *= 1.6f;
    }

    if (state == 0) {
        if (playerPos.x < sprite.getPosition().x) sprite.move(-speed * deltaTime, 0.0f);
        else sprite.move(speed * deltaTime, 0.0f);

        if (phase == 1) sprite.setColor(sf::Color::White);
        else sprite.setColor(sf::Color(255, 100, 100));

        shootTimer += deltaTime;
        float cooldown = (phase == 1) ? 3.5f : 2.0f;

        if (shootTimer >= cooldown) {
            state = 1;
            stateTimer = 0.6f;
        }
    }
    else if (state == 1) {
        stateTimer -= deltaTime;
        sprite.setColor(sf::Color(0, 255, 255)); // Miga przed strzałem

        if (stateTimer <= 0.f) {
            sf::Vector2f shootOrigin = sprite.getPosition() + sf::Vector2f(70.f, 50.f);

            // ZMIANA: Przekazujemy naszą teksturę kuli do pocisku
            bullets.push_back(BossBullet(shootOrigin.x, shootOrigin.y, playerPos, *bulletTexture));

            if (phase == 2) {
                sf::Vector2f spreadPos = playerPos;
                spreadPos.y -= 150.f;
                bullets.push_back(BossBullet(shootOrigin.x, shootOrigin.y, spreadPos, *bulletTexture));
            }

            shootTimer = 0.f;
            state = 0;
        }
    }

    for (auto& b : bullets) b.update(deltaTime);
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const BossBullet& b) { return !b.isAlive(); }), bullets.end());
}

void Boss::render(sf::RenderTarget& target) {
    for (auto& b : bullets) b.render(target);
    target.draw(sprite);
}

void Boss::takeDamage() { hp -= 25; }
bool Boss::isAlive() const { return hp > 0; }
sf::FloatRect Boss::getGlobalBounds() const { return sprite.getGlobalBounds(); }
sf::Vector2f Boss::getPosition() const { return sprite.getPosition(); }