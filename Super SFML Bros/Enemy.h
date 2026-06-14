#pragma once
#include "Entity.h"

class Enemy : public Entity
{
private:
    sf::RectangleShape shape; // Niewidzialny hitbox
    sf::Sprite sprite;        // Nasza grafika potwora
    float speed;
    int direction;
    bool alive;
public:
    // Dodano teksturę do konstruktora
    Enemy(float startX, float startY, const sf::Texture& tex);

    void update(sf::Time dt, const std::vector<sf::RectangleShape>& platforms) override;
    void render(sf::RenderWindow& window) override;
    sf::FloatRect getGlobalBounds() const;
    bool isAlive() const { return alive; }
    void die() { alive = false; }
};

class BossBullet {
private:
    sf::Sprite sprite; // ZMIANA: Mamy teraz Sprite zamiast CircleShape
    sf::Vector2f velocity;
    float lifetime;
    bool alive;

public:
    // ZMIANA: Konstruktor przyjmuje teksturę
    BossBullet(float x, float y, sf::Vector2f targetPos, const sf::Texture& tex);
    void update(float dt);
    void render(sf::RenderTarget& target);
    sf::FloatRect getBounds() const;
    bool isAlive() const { return alive; }
    void destroy() { alive = false; }
};

// ==========================================
// --- BOSS ---
// ==========================================
class Boss {
private:
    sf::Sprite sprite;
    int hp;
    float speed;
    int phase;

    int state;
    float stateTimer;
    float shootTimer;
    std::vector<BossBullet> bullets;

    const sf::Texture* bulletTexture; // NOWE: Boss trzyma wskaźnik na teksturę swoich kul

public:
    // ZMIANA: Konstruktor przyjmuje drugą teksturę dla kuli
    Boss(float startX, float startY, const sf::Texture& tex, const sf::Texture& bulletTex);

    void updateBoss(float deltaTime, const std::vector<sf::RectangleShape>& platforms, sf::Vector2f playerPos);
    void render(sf::RenderTarget& target);
    void takeDamage();
    bool isAlive() const;
    sf::FloatRect getGlobalBounds() const;
    sf::Vector2f getPosition() const;
    int getHp() const { return hp; }
    void setHp(int newHp) { hp = newHp; }
    std::vector<BossBullet>& getBullets() { return bullets; }
};

