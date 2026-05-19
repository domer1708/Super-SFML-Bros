#include "Entity.h"

Player::Player() // tworzymy gracza z jakimiś parametrami
{
	isJumping = false;
	position = sf::Vector2f(100.f, ground_height);
	velocity = sf::Vector2f(0.f, 0.f);

	shape.setPosition(position);
	shape.setFillColor(sf::Color::Red);
	shape.setSize(sf::Vector2f(50.f, 50.f));
}

void Player::handleEvent(sf::Event& event) // obsługa skoku
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

void Player::update(sf::Time dt)  // fizyka i ruch
{
	float playerSpeed = 300.f;
	float gravity = 980.f;
	velocity.x = 0.f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		velocity.x += playerSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		velocity.x -= playerSpeed;
	}

	velocity.y += gravity * dt.asSeconds();
	position += velocity * dt.asSeconds();

	if (position.y >= ground_height)
	{
		position.y = ground_height;
		velocity.y = 0.f;
		isJumping = false;
	}
	shape.setPosition(position);
}

void Player::render(sf::RenderWindow& window)
{
	window.draw(shape);
}

sf::Vector2f Player::getPosition() const
{
	return position;
}