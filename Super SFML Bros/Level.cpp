#include "Level.h"

bool Level::loadFromFile(const std::string& filename, const sf::Texture& mushroomTex, const sf::Texture& platformTex, const sf::Texture& doorTex, const sf::Texture& trapTex, const sf::Texture& keyTex)
{
    platforms.clear();
    stars.clear();
    traps.clear();
    portals.clear();
    enemies.clear();
    mushrooms.clear();
    checkpoints.clear();
    vanishingPlatforms.clear();
    movingPlatforms.clear();
    turrets.clear();
    bullets.clear();
    iceBlocks.clear();
    elevators.clear();
    bosses.clear();

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

                // Teraz platformTex jest poprawnie przekazywane do funkcji!
                block.setTexture(&platformTex);
                block.setFillColor(sf::Color::White);

                block.setPosition(x * tile_size, y * tile_size);
                platforms.push_back(block);
            }
            else if (tile == 'P') playerSpawnPosition = sf::Vector2f(x * tile_size, y * tile_size);
            else if (tile == 'S') stars.push_back(Star(x * tile_size, y * tile_size));
            else if (tile == '^') traps.push_back(Trap(x * tile_size, y * tile_size, trapTex));
            else if (tile == 'F') portals.push_back(Portal(x * tile_size, y * tile_size, doorTex, true));
            else if (tile == 'T') trampolines.push_back(Trampoline(x * tile_size, y * tile_size));
            else if (tile == 'K') keys.push_back(Key(x * tile_size, y * tile_size, keyTex));
            else if (tile == 'E') enemies.push_back(Enemy(x * tile_size, y * tile_size));
            else if (tile == 'M') mushrooms.push_back(Mushroom(x * tile_size, y * tile_size, mushroomTex));
            else if (tile == 'C') checkpoints.push_back(Checkpoint(x * tile_size, y * tile_size));
            else if (tile == 'Z') vanishingPlatforms.push_back(VanishingPlatform(x * tile_size, y * tile_size));
            else if (tile == 'R') movingPlatforms.push_back(MovingPlatform(x * tile_size, y * tile_size));
            else if (tile == 'W') turrets.push_back(Turret(x * tile_size, y * tile_size));
            else if (tile == 'I')
            {
                sf::RectangleShape ice;
                ice.setSize(sf::Vector2f(tile_size, tile_size));
                ice.setFillColor(sf::Color(150, 255, 255)); // Lód
                ice.setPosition(x * tile_size, y * tile_size);
                iceBlocks.push_back(ice);
            }
            else if (tile == 'V') elevators.push_back(Elevator(x * tile_size, y * tile_size));
            else if (tile == 'B') bosses.push_back(Boss(x * tile_size, y * tile_size));
        }
        y++;
    }
    file.close();
    return true;
}

const std::vector<sf::RectangleShape>& Level::getPlatforms() const { return platforms; }
sf::Vector2f Level::getPlayerSpawn() const { return playerSpawnPosition; }

void Level::render(sf::RenderWindow& window)
{
    for (auto& i : platforms) window.draw(i);
    for (auto& trap : traps) trap.render(window);
    for (auto& star : stars) star.render(window);
    for (auto& mushroom : mushrooms) mushroom.render(window);
    for (auto& t : trampolines) t.render(window);
    for (auto& k : keys) k.render(window);
    for (auto& c : checkpoints) c.render(window);
    for (auto& vp : vanishingPlatforms) vp.render(window);
    for (auto& mp : movingPlatforms) mp.render(window);
    for (auto& t : turrets) t.render(window);
    for (auto& b : bullets) b.render(window);
    for (auto& ice : iceBlocks) window.draw(ice);
    for (auto& v : elevators) v.render(window);
    for (auto& b : bosses) b.render(window);
}

std::vector<Star>& Level::getStars() { return stars; }
const std::vector<Trap>& Level::getTraps() const { return traps; }

void Level::removeCollectedStars() {
    stars.erase(std::remove_if(stars.begin(), stars.end(), [](const Star& s) { return s.isCollected(); }), stars.end());
}

std::vector<Portal>& Level::getPortals() { return portals; }
std::vector<Enemy>& Level::getEnemies() { return enemies; }

void Level::removeCollectedKeys() {
    keys.erase(std::remove_if(keys.begin(), keys.end(), [](const Key& k) { return k.isCollected(); }), keys.end());
}

void Level::updateLevelElements(sf::Time dt) {
    for (auto& vp : vanishingPlatforms) vp.update(dt);
    for (auto& mp : movingPlatforms) mp.update(dt);
    for (auto& v : elevators) v.update(dt);

    for (auto& t : turrets) {
        if (t.updateAndCheckShoot(dt)) {
            spawnBullet(t.getBounds().left, t.getBounds().top);
        }
    }

    for (auto& b : bullets) b.update(dt);
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.isAlive(); }), bullets.end());
}