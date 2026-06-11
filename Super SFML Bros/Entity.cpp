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
    // ---------------------------------------------------------

    invincibilityTimer = sf::Time::Zero;
    baseColor = sf::Color::White; // Zmieniamy bazowy na biały, by tekstura miała naturalne kolory

    // --- FIZYCZNE PUDEŁKO KOLIZJI (Niewidzialne pod spodem) ---
    shape.setFillColor(sf::Color::Transparent); // Robimy je przezroczyste
    shape.setSize(sf::Vector2f(50.f, 50.f));
    sf::Image image;
    // --- ŁADOWANIE GRAFIKI MARIO ---
    if (texture.loadFromFile("pliki/mario_spritesheet.png"))
    {
		image.createMaskFromColor(sf::Color::White);
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, 16, 16)); // Pierwsza klatka - stanie
        sprite.setScale(3.125f, 3.125f);                  // Skalowanie 16px -> 50px
    }
    else
    {
        std::cout << "Blad: Nie udalo sie pliku pliki/mario_spritesheet.png" << std::endl;
    }

    animationFrame = 0;
    isFacingRight = true;

    // Ładowanie dźwięku przy skoku
    if (jumpBuffer.loadFromFile("pliki/jump.ogg"))
    {
        jumpSound.setBuffer(jumpBuffer);
        jumpSound.setVolume(0.f); // Wyciszamy całkowicie na "suchy" start
        jumpSound.play();         
        jumpSound.setVolume(50.f);
    }
    else
    {
        std::cout << "Blad: Nie udalo sie wczytac pliki/jump.wav" << std::endl;
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
    // --- SYSTEM NIEŚMIERTELNOŚCI I MIGANIA GRAFIKI ---
    if (invincibilityTimer > sf::Time::Zero)
    {
        invincibilityTimer -= dt;

        // Miganie za pomocą przezroczystości Sprite'a
        if (static_cast<int>(invincibilityTimer.asMilliseconds() / 100) % 2 == 0)
            sprite.setColor(sf::Color(baseColor.r, baseColor.g, baseColor.b, 100)); // Półprzezroczysty Mario
        else
            sprite.setColor(baseColor);
    }
    else
    {
        sprite.setColor(baseColor);
    }

    // --- SYSTEM ROZPĘDU I ŚLIZGANIA ---
    float gravity = 1000.f;
    float dtSeconds = dt.asSeconds();

    float directionX = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        directionX += 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        directionX -= 1.f;
    }

    if (directionX != 0.f)
    {
        velocity.x += directionX * acceleration * dtSeconds;
        
        if (velocity.x > maxSpeed)  velocity.x = maxSpeed;
        if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;
    }
    else
    {
        velocity.x -= velocity.x * friction * dtSeconds;
        if (std::abs(velocity.x) < 10.f)
        {
            velocity.x = 0.f;
        }
    }

    // Ruch w osi X i kolizje
    position.x += velocity.x * dt.asSeconds();
    shape.setPosition(position);

    for (const auto& i : platforms)
    {
        if (shape.getGlobalBounds().intersects(i.getGlobalBounds()))
        {
            if (velocity.x > 0)
            {
                position.x = i.getGlobalBounds().left - shape.getGlobalBounds().width;
            }
            else if (velocity.x < 0)
            {
                position.x = i.getGlobalBounds().left + i.getGlobalBounds().width;
            }
            velocity.x = 0.f;
            shape.setPosition(position);
        }
    }

    // Ruch w osi Y i kolizje
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

    // --- ANIMACJA I OBRACANIE MARIO ---
    if (velocity.x > 1.f) isFacingRight = true;
    else if (velocity.x < -1.f) isFacingRight = false;

    if (isJumping || std::abs(velocity.y) > 50.f) 
    {
        // Klatka skoku (X = 96 w oryginalnym arkuszu NES)
        sprite.setTextureRect(sf::IntRect(96, 0, 16, 16));
    }
    else if (std::abs(velocity.x) > 10.f) 
    {
        // Zmiana klatki biegu co 0.08 sekundy
        if (animationClock.getElapsedTime().asSeconds() > 0.08f) {
            animationFrame = (animationFrame + 1) % 3; // 3 klatki biegu
            animationClock.restart();
        }
        int frameX = 16 + (animationFrame * 16); // Klatki startują od 16px
        sprite.setTextureRect(sf::IntRect(frameX, 0, 16, 16));
    }
    else 
    {
        sprite.setTextureRect(sf::IntRect(0, 0, 16, 16)); // Stanie w miejscu
    }

    // Fizyczne obracanie grafiki (lustro)
    if (!isFacingRight) 
    {
        sprite.setScale(-3.125f, 3.125f);
        sprite.setOrigin(16.f, 0.f); // Zapobiega przeskokom pozycji
    } 
    else 
    {
        sprite.setScale(3.125f, 3.125f);
        sprite.setOrigin(0.f, 0.f);
    }
}

void Player::render(sf::RenderWindow& window)
{
    // Przyklejamy Sprite do aktualnej pozycji fizycznej
    sprite.setPosition(shape.getPosition());
    
    // Rysujemy animowanego Mario
    window.draw(sprite);
}

sf::Vector2f Player::getPosition() const
{
    return position;
}

void Player::setColor(sf::Color color)
{
    if (color == sf::Color::Red) {
        baseColor = sf::Color(254, 254, 254); // Prawie biały (dla oka identyczny, ale oszukuje maskę tła)
    }
    else if (color == sf::Color::Green) {
        baseColor = sf::Color(120, 255, 120); 
    }
    else if (color == sf::Color::Blue) {
        baseColor = sf::Color(120, 120, 255); 
    }
    else 
	{
        baseColor = color; 
    }

    sprite.setColor(baseColor);
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

int Player::getHp() const
{
    return hp;
}

bool Player::isAlive() const
{
    return hp > 0;
}

void Player::bounce()
{
    velocity.y = -600.f;
    isJumping = true;
}

void Player::setSuper(bool status)
{
    hasSuperPower = status;
    if (status) setColor(sf::Color(255, 215, 0)); // Złoty Mario po zjedzeniu grzyba!
    else setColor(sf::Color::White);              // Powrót do naturalnych kolorów
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
