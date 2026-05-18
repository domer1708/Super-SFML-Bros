#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

using namespace std;

class Entity
{
protected:
	sf::Vector2f velocity;
	sf::Vector2f position;

public:
	virtual ~Entity() = default;
	virtual void update(sf::Time dt) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
};

class Player : public Entity
{
private:
	sf::RectangleShape shape;
	bool isJumping;

public:
	Player();

	void handleEvent(sf::Event& event);
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};