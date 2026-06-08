#include "Entity.h"

Player::Player()
{
	isJumping = false;
	velocity = sf::Vector2f(0.f, 0.f);

	hp = 3;                                 // Startujemy z 3 życiami
	invincibilityTimer = sf::Time::Zero;    // Na start nie jesteśmy nieśmiertelni

	shape.setFillColor(sf::Color::Red);
	shape.setSize(sf::Vector2f(50.f, 50.f));
}

void Player::setPosition(float x, float y)
{
	position = sf::Vector2f(x, y);
	shape.setPosition(position);
}

void Player::handleEvent(sf::Event& event) // obsługa skoku
{
	if(event.type == sf::Event::KeyPressed)
	{
		if((event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) && !isJumping)
		{
			velocity.y = -600.f;
			isJumping = true;
		}
	}
}

void Player::update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms)  // fizyka i ruch
{
	// --- SYSTEM NIEŚMIERTELNOŚCI I MIGANIA ---
	if (invincibilityTimer > sf::Time::Zero)
	{
		invincibilityTimer -= dt; // Czas leci w dół

		// Co 100 milisekund zmieniamy przezroczystość (miganie)
		if (static_cast<int>(invincibilityTimer.asMilliseconds() / 100) % 2 == 0)
			shape.setFillColor(sf::Color(255, 0, 0, 100)); // Półprzezroczysty
		else
			shape.setFillColor(sf::Color::Red); // Normalny
	}
	else
	{
		shape.setFillColor(sf::Color::Red); // Czas minął, upewnij się że kolor jest normalny
	}

	float playerSpeed = 300.f;
	float gravity = 1000.f;
	velocity.x = 0.f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		velocity.x += playerSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		velocity.x -= playerSpeed;
	}

	position.x += velocity.x * dt.asSeconds();  // ruch na osi x
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

	velocity.y += gravity * dt.asSeconds();    // ruch na osi y
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
    shape.setFillColor(color);
}

void Player::takeDamage(int damage)
{
	// Otrzymujesz obrażenia TYLKO wtedy, gdy zegar nieśmiertelności spadł do zera
	if (invincibilityTimer <= sf::Time::Zero)
	{
		hp -= damage;
		invincibilityTimer = sf::seconds(1.5f); // Dajemy 1.5 sekundy ochrony!

		velocity.y = -400.f; // Odrzut w górę od kolców
		isJumping = true;

		std::cout << "Aua! Zostalo zyc: " << hp << std::endl; // Wypisze w czarnej konsoli
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
	velocity.y = -600.f; // Wybija gracza z powrotem w powietrze
	isJumping = true;
}
