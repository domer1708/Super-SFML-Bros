#pragma once

#include "Elements.h"
#include "Enemy.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
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
    sf::Vector2f playerSpawnPosition;      
    static constexpr float tile_size = 50.f;

public:
    Level() = default;

    bool loadFromFile(const std::string& filename);
    const std::vector<sf::RectangleShape>& getPlatforms() const;
    sf::Vector2f getPlayerSpawn() const;
    void render(sf::RenderWindow& window);
    std::vector<Star>& getStars();
    const std::vector<Trap>& getTraps() const;
    void removeCollectedStars();
    std::vector<Portal>& getPortals();
    std::vector<Enemy>& getEnemies();
};