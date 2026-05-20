#include "Level.h"

bool Level::loadFromFile(const std::string& filename)
{
    platforms.clear();
    playerSpawnPosition = sf::Vector2f(100.f, 100.f);

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Blad: Nie udalo sie otworzyc pliku poziomu: " << filename << std::endl;
        return false;
    }

    std::string line;
    int y = 0;

    while (std::getline(file, line))
    {
        for (int x = 0; x < line.length(); x++)
        {
            char tile = line[x];
            if (tile == 'X')
            {
                sf::RectangleShape block;
                block.setSize(sf::Vector2f(tile_size, tile_size));
                block.setFillColor(sf::Color::Green);
                block.setPosition(x * tile_size, y * tile_size);
                platforms.push_back(block);
            }
            else if (tile == 'P')
            {
                playerSpawnPosition = sf::Vector2f(x * tile_size, y * tile_size);
            }
        }
        y++;
    }
    file.close();
    return true;
}

const std::vector<sf::RectangleShape>& Level::getPlatforms() const
{
    return platforms;
}

sf::Vector2f Level::getPlayerSpawn() const
{
    return playerSpawnPosition;
}

void Level::render(sf::RenderWindow& window)
{
    for (auto& i : platforms)
    {
        window.draw(i);
    }
}