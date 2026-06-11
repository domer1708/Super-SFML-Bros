#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <SFML/Audio.hpp>
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
    sf::RectangleShape shape; // Niewidzialny hitbox (fizyka)
    sf::Texture texture;      // Załadowana tekstura Mario
    sf::Sprite sprite;        // Wyświetlany Mario

    sf::Clock animationClock;
    int animationFrame;
    bool isFacingRight;
    bool isJumping;
    int hp;
    sf::Time invincibilityTimer;
    sf::Color baseColor;
    int score;
    bool hasSuperPower;

    // FIZYKA RUCHU
    float acceleration = 1500.f;
    float maxSpeed = 350.f;
    float friction = 8.f;
    int killCombo = 0;
    int starsCollected = 0;
    bool hasKeyVar;
    sf::Vector2f checkpointPosition;
    bool hasActiveCheckpoint;

    sf::SoundBuffer jumpBuffer;
    sf::Sound jumpSound;

public:
    Player();
    void handleEvent(sf::Event& event); // obsługa skoku
    void update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms) override; // aktualizacja fizyki i animacji
    void render(sf::RenderWindow& window) override; // rysowanie grafiki
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
    void superBounce();
    bool hasCheckpoint() const { return hasActiveCheckpoint; }
    sf::Vector2f getCheckpointPos() const { return checkpointPosition; }
    void setCheckpoint(sf::Vector2f pos);
    void resetCheckpoint() { hasActiveCheckpoint = false; }
    void resetVelocity() { velocity = sf::Vector2f(0.f, 0.f); }
    void setFriction(float f) { friction = f; }
    float getFriction() const { return friction; }
    void resetJumping() { isJumping = false; velocity.y = 0.f; }
};