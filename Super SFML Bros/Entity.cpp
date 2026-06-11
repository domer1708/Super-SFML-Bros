#include "Entity.h"
#include <iostream>
#include <cmath>

Player::Player()
{
    isJumping = false;
    hasSuperPower = false;
    velocity = sf::Vector2f(0.f, 0.f);
    hp = 3;
    score = 0;
    starsCollected = 0;
    killCombo = 0;
    hasKeyVar = false;
    hasActiveCheckpoint = false;
    checkpointPosition = sf::Vector2f(0.f, 0.f);

    invincibilityTimer = sf::Time::Zero;
    baseColor = sf::Color::White;

    // --- FIZYCZNY HITBOX ---
    shape.setFillColor(sf::Color::Transparent);
    shape.setSize(sf::Vector2f(50.f, 50.f));

    // --- ŁADOWANIE OSOBNYCH KLATEK ANIMACJI ---
    sf::Image imgStand, imgW1, imgW2, imgW3, imgJump;
    
    // Funkcja pomocnicza lambda (skraca kod i dba o to samo dla każdego pliku)
    auto loadTexture = [](sf::Image& img, sf::Texture& tex, const std::string& path) {
        if (img.loadFromFile(path)) {
            img.createMaskFromColor(sf::Color::White); // Usunięcie białego tła
            tex.loadFromImage(img);
            tex.setSmooth(false); // Wyłączamy rozmywanie pikseli (niezwykle ważne w pixel-arcie!)
        } else {
            std::cout << "Blad: Nie udalo sie zaladowac " << path << std::endl;
        }
    };

    loadTexture(imgStand, texStand, "pliki/mario_stand.png");
    loadTexture(imgW1, texWalk1, "pliki/mario_walk1.png");
    loadTexture(imgW2, texWalk2, "pliki/mario_walk2.png");
    loadTexture(imgW3, texWalk3, "pliki/mario_walk3.png");
    loadTexture(imgJump, texJump, "pliki/marioj_jump.png");

    // Ustawiamy domyślną teksturę na stanie
    sprite.setTexture(texStand);

    animationFrame = 0;
    isFacingRight = true;

    if (jumpBuffer.loadFromFile("pliki/jump.ogg"))
    {
        jumpSound.setBuffer(jumpBuffer);
        jumpSound.setVolume(50.f);
    }
}

void Player::setPosition(float x, float y)
{
    position = sf::Vector2f(x, y);
    shape.setPosition(position);
}

void Player::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if ((event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) && !isJumping)
        {
            velocity.y = -600.f;
            isJumping = true;
            jumpSound.play();
        }
    }
}

void Player::update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms)
{
    // --- SYSTEM NIEŚMIERTELNOŚCI I MIGANIA ---
    if (invincibilityTimer > sf::Time::Zero)
    {
        invincibilityTimer -= dt;
        if (static_cast<int>(invincibilityTimer.asMilliseconds() / 100) % 2 == 0)
            sprite.setColor(sf::Color(255, 255, 255, 100));
        else
            sprite.setColor(baseColor);
    }
    else
    {
        if(hasSuperPower) sprite.setColor(sf::Color(255, 215, 0));
        else sprite.setColor(baseColor);
    }

    float gravity = 1000.f;
    float dtSeconds = dt.asSeconds();
    float directionX = 0.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) directionX += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) directionX -= 1.f;

    if (directionX != 0.f)
    {
        velocity.x += directionX * acceleration * dtSeconds;
        if (velocity.x > maxSpeed)  velocity.x = maxSpeed;
        if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;
    }
    else
    {
        velocity.x -= velocity.x * friction * dtSeconds;
        if (std::abs(velocity.x) < 10.f) velocity.x = 0.f;
    }

    position.x += velocity.x * dt.asSeconds();
    shape.setPosition(position);

    for (const auto& i : platforms)
    {
        if (shape.getGlobalBounds().intersects(i.getGlobalBounds()))
        {
            if (velocity.x > 0) position.x = i.getGlobalBounds().left - shape.getGlobalBounds().width;
            else if (velocity.x < 0) position.x = i.getGlobalBounds().left + i.getGlobalBounds().width;
            velocity.x = 0.f;
            shape.setPosition(position);
        }
    }

    velocity.y += gravity * dt.asSeconds();
    position.y += velocity.y * dt.asSeconds();
    shape.setPosition(position);
    isJumping = true;

    for (const auto& i : platforms)
    {
        if (shape.getGlobalBounds().intersects(i.getGlobalBounds()))
        {
            if (velocity.y > 0)
            {
                position.y = i.getGlobalBounds().top - shape.getGlobalBounds().height;
                isJumping = false;
                resetCombo();
            }
            else if (velocity.y < 0)
            {
                position.y = i.getGlobalBounds().top + i.getGlobalBounds().height;
            }
            velocity.y = 0.f;
            shape.setPosition(position);
        }
    }

    if (velocity.x > 1.f) isFacingRight = true;
    else if (velocity.x < -1.f) isFacingRight = false;


    // =========================================================================
    // PROSTA ANIMACJA - ZMIANA TEKSTUR
    // =========================================================================
    if (isJumping || std::abs(velocity.y) > 50.f)
    {
        // true na końcu resetuje TextureRect, żeby dopasować się do rozmiaru nowego pliku
        sprite.setTexture(texJump, true); 
    }
    else if (std::abs(velocity.x) > 10.f)
    {
        if (animationClock.getElapsedTime().asSeconds() > 0.08f) {
            animationFrame = (animationFrame + 1) % 3;
            animationClock.restart();
        }
        if (animationFrame == 0) sprite.setTexture(texWalk1, true);
        else if (animationFrame == 1) sprite.setTexture(texWalk2, true);
        else sprite.setTexture(texWalk3, true);
    }
    else
    {
        sprite.setTexture(texStand, true);
    }

    // --- SKALOWANIE I OBRACANIE ---
    // Pobieramy rozmiar aktualnie ustawionej tekstury z pliku
    sf::Vector2u currentTexSize = sprite.getTexture()->getSize();
    
    // Zabezpieczenie przed błędem, gdyby tekstura nie wczytała się poprawnie (dzielenie przez zero)
    if (currentTexSize.y > 0) 
    {
        float scaleFactor = 60.f / static_cast<float>(currentTexSize.y); // Mario ma 60px wysokości

        if (!isFacingRight)
        {
            sprite.setScale(-scaleFactor, scaleFactor);
            sprite.setOrigin(static_cast<float>(currentTexSize.x), 0.f);
        }
        else
        {
            sprite.setScale(scaleFactor, scaleFactor);
            sprite.setOrigin(0.f, 0.f);
        }
    }
}

void Player::render(sf::RenderWindow& window)
{
    // Rysowanie z zaokrąglaniem współrzędnych zapobiega błędom sub-pixel renderowania
    // (migotaniu i powstawaniu dziwnych pasów na krawędziach)
    float drawX = std::round(shape.getPosition().x - 8.f);
    float drawY = std::round(shape.getPosition().y - 10.f);

    sprite.setPosition(drawX, drawY);
    window.draw(sprite);
}

sf::Vector2f Player::getPosition() const { return position; }

void Player::setColor(sf::Color color)
{
    baseColor = color; // NAPRAWIONE: Zapisuje w pamięci wybrany kolor (np. zielony dla Luigiego)
    if(!hasSuperPower) sprite.setColor(baseColor);
}

void Player::takeDamage(int damage)
{
    if (invincibilityTimer <= sf::Time::Zero)
    {
        hp -= damage;
        invincibilityTimer = sf::seconds(1.5f);
        velocity.y = -400.f;
        isJumping = true;
    }
}

int Player::getHp() const { return hp; }
bool Player::isAlive() const { return hp > 0; }

void Player::bounce()
{
    velocity.y = -600.f;
    isJumping = true;
}

void Player::setSuper(bool status)
{
    hasSuperPower = status;
    if (status) sprite.setColor(sf::Color(255, 215, 0));
    else sprite.setColor(baseColor); // Gdy traci moc, wraca do oryginalnego koloru (np. Zielonego)
}

void Player::superBounce()
{
    velocity.y = -1100.f;
    isJumping = true;
}

void Player::setCheckpoint(sf::Vector2f pos)
{
    checkpointPosition = pos;
    hasActiveCheckpoint = true;
}
