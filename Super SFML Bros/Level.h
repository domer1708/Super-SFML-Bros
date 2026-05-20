#pragma once

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
    sf::Vector2f playerSpawnPosition;      
    static constexpr float tile_size = 50.f;

public:
    Level() = default;

    bool loadFromFile(const std::string& filename);
    const std::vector<sf::RectangleShape>& getPlatforms() const;
    sf::Vector2f getPlayerSpawn() const;
    void render(sf::RenderWindow& window);
};