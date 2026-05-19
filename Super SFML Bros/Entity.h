#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <memory>

class Entity // klasa bazowa postaci 
{
protected:
	sf::Vector2f velocity; // wektor prędkości
	sf::Vector2f position; // wektor pozycji
	static constexpr float ground_height = 500.f; // stała wysokość podłogi (tymczasowa)

public:
	virtual ~Entity() = default;
	virtual void update(sf::Time dt) = 0; // aktualizacjia fizyki i ruchu
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
	void update(sf::Time dt) override; // aktualizacja fizyki i ruchu
	void render(sf::RenderWindow& window) override; // rysowanie
	sf::Vector2f getPosition() const;
};