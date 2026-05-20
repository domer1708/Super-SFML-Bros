#pragma once

#include "Level.h"

class Entity // klasa bazowa postaci 
{
protected:
	sf::Vector2f velocity; // wektor prędkości
	sf::Vector2f position; // wektor pozycji

public:
	virtual ~Entity() = default;
	virtual void update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms) = 0; // aktualizacjia fizyki i ruchu
	virtual void render(sf::RenderWindow& window) = 0; // rysowanie
};

class Player : public Entity
{
private:
	sf::RectangleShape shape; // kształt gracza (do wymiany)
	bool isJumping;

public:
	Player();

	void handleEvent(sf::Event& event); // obsługa skoku
	void update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms) override; // aktualizacja fizyki i ruchu
	void render(sf::RenderWindow& window) override; // rysowanie

	sf::Vector2f getPosition() const;
	void setPosition(float x, float y);
};