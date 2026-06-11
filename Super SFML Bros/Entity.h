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
	int killCombo = 0; // Licznik zabitych potworów bez dotykania ziemi
	int starsCollected = 0; // licznik gwiazdek
	bool hasKeyVar;
	sf::Vector2f checkpointPosition;
	bool hasActiveCheckpoint;

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
	void setSuper(bool status);
	void incrementCombo() { killCombo++; }
    void resetCombo() { killCombo = 0; }
    int getCombo() const { return killCombo; }
	void addStar() { starsCollected++; }
    int getStarsCount() const { return starsCollected; }
	void setStarsCount(int stars) { starsCollected = stars; }
	bool hasKey() const { return hasKeyVar; }
	void collectKey() { hasKeyVar = true; }
	void useKey() { hasKeyVar = false; }
	void setKey(bool k) { hasKeyVar = k; }
	void superBounce(); // Skok z trampoliny
	bool hasCheckpoint() const { return hasActiveCheckpoint; }
	sf::Vector2f getCheckpointPos() const { return checkpointPosition; }
	void setCheckpoint(sf::Vector2f pos);
	void resetCheckpoint() { hasActiveCheckpoint = false; }
	void resetVelocity() { velocity = sf::Vector2f(0.f, 0.f); }
	void setFriction(float f) { friction = f; }
	float getFriction() const { return friction; }
};