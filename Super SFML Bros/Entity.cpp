#include "Entity.h"
#include <iostream>

Player::Player()
{
	isJumping = false;
	hasSuperPower = false;
	velocity = sf::Vector2f(0.f, 0.f);

	hp = 3;
	score = 0;
	invincibilityTimer = sf::Time::Zero;

	baseColor = sf::Color::Red;      // Zapisujemy domyślny kolor na starcie
	shape.setFillColor(baseColor);
	shape.setSize(sf::Vector2f(50.f, 50.f));
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
		}
	}
}

void Player::update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms)
{
	// --- SYSTEM NIEŚMIERTELNOŚCI I MIGANIA ---
	if (invincibilityTimer > sf::Time::Zero)
	{
		invincibilityTimer -= dt;

		// Co 100 milisekund zmieniamy przezroczystość (miganie)
		if (static_cast<int>(invincibilityTimer.asMilliseconds() / 100) % 2 == 0)
			shape.setFillColor(sf::Color(baseColor.r, baseColor.g, baseColor.b, 100)); // Półprzezroczysty, ale zachowuje kolor!
		else
			shape.setFillColor(baseColor); // Normalny
	}
	else
	{
		shape.setFillColor(baseColor); // Wracamy do zapamiętanego koloru!
	}

	/*float playerSpeed = 300.f;
	float gravity = 1000.f;
	velocity.x = 0.f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		velocity.x += playerSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		velocity.x -= playerSpeed;
	}*/
    // --- NOWY SYSTEM ROZPĘDU I ŚLIZGANIA ---
    float gravity = 1000.f;
    float dtSeconds = dt.asSeconds();

    // 1. Zbieranie kierunku ruchu
    float directionX = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        directionX += 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        directionX -= 1.f;
    }

    // 2. Płynne przyspieszanie lub hamowanie
    if (directionX != 0.f)
    {
        // Gracz naciska klawisz -> przyspieszamy
        velocity.x += directionX * acceleration * dtSeconds;
        
        // Blokada maksymalnej prędkości (maxSpeed = 350.f)
        if (velocity.x > maxSpeed)  velocity.x = maxSpeed;
        if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;
    }
    else
    {
        // Gracz puścił klawisze -> tarcie (friction = 8.f) robi ślizg
        velocity.x -= velocity.x * friction * dtSeconds;

        // Jeśli prędkość spadnie blisko zera, zatrzymujemy całkowicie
        if (std::abs(velocity.x) < 10.f)
        {
            velocity.x = 0.f;
        }
    }
    // ----------------------------------------

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
}

void Player::render(sf::RenderWindow& window)
{
	window.draw(shape);
}

sf::Vector2f Player::getPosition() const
{
	return position;
}

void Player::setColor(sf::Color color)
{
	baseColor = color;
	shape.setFillColor(baseColor);
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