#pragma once
#include "Elements.h"
#include "Enemy.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class Level
{
private:
    std::vector<sf::RectangleShape> platforms;
    std::vector<Star> stars;
    std::vector<Trap> traps;
    std::vector<Portal> portals;
    std::vector<Enemy> enemies;
    std::vector<Mushroom> mushrooms;
    std::vector<Trampoline> trampolines;
    std::vector<Key> keys;
    std::vector<Checkpoint> checkpoints;
    std::vector<VanishingPlatform> vanishingPlatforms;
    std::vector<MovingPlatform> movingPlatforms;
    std::vector<Turret> turrets;
    std::vector<Bullet> bullets;
    std::vector<sf::RectangleShape> iceBlocks;
    std::vector<Elevator> elevators;
    std::vector<Boss> bosses;
    sf::Vector2f playerSpawnPosition;
    static constexpr float tile_size = 50.f;

public:
    Level() = default;

    // --- UJEDNOLICONA DEKLARACJA ---
    bool loadFromFile(const std::string& filename, const sf::Texture& mushroomTex, const sf::Texture& platformTex, const sf::Texture& doorTex, const sf::Texture& trapTex);

    const std::vector<sf::RectangleShape>& getPlatforms() const;
    sf::Vector2f getPlayerSpawn() const;
    void render(sf::RenderWindow& window);
    std::vector<Star>& getStars();
    const std::vector<Trap>& getTraps() const;
    void removeCollectedStars();
    std::vector<Portal>& getPortals();
    std::vector<Enemy>& getEnemies();
    std::vector<Mushroom>& getMushrooms() { return mushrooms; }
    const std::vector<Trampoline>& getTrampolines() const { return trampolines; }
    std::vector<Key>& getKeys() { return keys; }
    void removeCollectedKeys();
    std::vector<Checkpoint>& getCheckpoints() { return checkpoints; }
    std::vector<VanishingPlatform>& getVanishingPlatforms() { return vanishingPlatforms; }
    std::vector<MovingPlatform>& getMovingPlatforms() { return movingPlatforms; }
    std::vector<Turret>& getTurrets() { return turrets; }
    std::vector<Bullet>& getBullets() { return bullets; }
    void spawnBullet(float x, float y) { bullets.push_back(Bullet(x, y)); }
    void updateLevelElements(sf::Time dt);
    const std::vector<sf::RectangleShape>& getIceBlocks() const { return iceBlocks; }
    std::vector<Elevator>& getElevators() { return elevators; }
    std::vector<Boss>& getBosses() { return bosses; }
};