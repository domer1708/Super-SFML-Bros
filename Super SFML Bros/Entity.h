#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

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
	int hp;                      
	sf::Time invincibilityTimer;
	sf::Color baseColor;
	int score;
	bool hasSuperPower;
	// --- NOWE ZMIENNE DO FIZYKI RUCHU ---
    float acceleration = 1500.f;  // Jak szybko gracz przyspiesza
    float maxSpeed = 350.f;      // Maksymalna prędkość biegu
    float friction = 8.f;


public:
	Player();

	void handleEvent(sf::Event& event); // obsługa skoku
	void update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms) override; // aktualizacja fizyki i ruchu
	void render(sf::RenderWindow& window) override; // rysowanie
    void setColor(sf::Color color);
	sf::Vector2f getPosition() const;
	void setPosition(float x, float y);
	sf::FloatRect getGlobalBounds() const { return shape.getGlobalBounds(); }
	void takeDamage(int damage);
	int getHp() const;        
	bool isAlive() const;
	sf::Vector2f getVelocity() const { return velocity; }
	void bounce(); 
	void setHp(int newHp) { hp = newHp; }
	int getScore() const { return score; }
	void addScore(int points) { score += points; }
	void setScore(int s) { score = s; }
	bool isSuper() const { return hasSuperPower; }
    void setSuper(bool status) 
	{ 
        hasSuperPower = status; 
		if(status) setColor(sf::Color(255, 215, 0)); // Złoty kolor gdy ma moc!
    	else setColor(sf::Color::Red); // Powrót do czerwonego (domyślny)
    }

};