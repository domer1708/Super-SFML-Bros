#include "State.h"
#include "Enemy.h"
#include "Elements.h"
#include <algorithm>
#include <iostream>
#include <fstream>

PlayState::PlayState(int characterIndex, bool loadFromSave)
{
    player = std::make_unique<Player>();
    camera.setSize(800.f, 600.f);

    isGameOver = false;
    isGameWon = false;
    isPaused = false;
    currentLevelNumber = 1;
    currentCharacterIndex = characterIndex;

    font.loadFromFile("pliki/PressStart2P-Regular.ttf");

    // Konfiguracja Napisów (Game Over itp.)
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER"); 
    gameOverText.setCharacterSize(60); 
    gameOverText.setFillColor(sf::Color::Red); 
    gameOverText.setStyle(sf::Text::Bold);
    gameWonText.setFont(font); 
    gameWonText.setString("WYGRALES GRE!"); 
    gameWonText.setCharacterSize(60); 
    gameWonText.setFillColor(sf::Color::Yellow); 
    gameWonText.setStyle(sf::Text::Bold);
    resetText.setFont(font); resetText.setString("Wcisnij ESC, aby wrocic do menu"); 
    resetText.setCharacterSize(20); 
    resetText.setFillColor(sf::Color::White);

    // --- HUD GWIAZDEK ---
    starText.setFont(font);
    starText.setCharacterSize(24);
    starText.setFillColor(sf::Color::Yellow);
    starText.setPosition(420.f, 20.f); // Prawy górny róg
    starText.setString("Gwiazdki: 0");
    // --- HUD PUNKTÓW (SCORE) ---
    scoreText.setFont(font);
    scoreText.setCharacterSize(16);
    scoreText.setFillColor(sf::Color::White); // Biały tekst, ładnie odetnie się od żółtych gwiazdek
    scoreText.setString("Score: 000000");
    if (!mushroomTexture.loadFromFile("pliki/mashroms.png"))
    {
        std::cout << "Blad ladowania pliku pliki/mushroom.png!" << std::endl;
    }
    // Próba wczytania z menu głównego
    bool loaded = false;
    if (loadFromSave) 
    {
        loaded = loadGame();
    }

    // Jeśli nowa gra, lub brak zapisu (fallback)
    if (!loaded) {
        if (characterIndex == 0) player->setColor(sf::Color::Red);
        else if (characterIndex == 1) player->setColor(sf::Color::Green);
        else if (characterIndex == 2) player->setColor(sf::Color::Blue);

        currentLevel.loadFromFile("pliki/level1.txt", mushroomTexture);
        player->setPosition(currentLevel.getPlayerSpawn().x, currentLevel.getPlayerSpawn().y);
    }

    // Geometria serca
    heartShape.setPointCount(6); heartShape.setPoint(0, sf::Vector2f(20.f, 10.f)); heartShape.setPoint(1, sf::Vector2f(30.f, 0.f)); heartShape.setPoint(2, sf::Vector2f(40.f, 12.f)); heartShape.setPoint(3, sf::Vector2f(20.f, 38.f)); heartShape.setPoint(4, sf::Vector2f(0.f, 12.f)); heartShape.setPoint(5, sf::Vector2f(10.f, 0.f));

    // Menu pauzy
    pauseMenu = std::make_unique<PauseMenu>(font);
    
    std::string bgName = "pliki/tlo" + std::to_string(currentLevelNumber) + ".png";
    if (!backgroundTexture.loadFromFile(bgName))
    {
        std::cout << "Blad ladowania pliku " << bgName << "!" << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture);
}

void PlayState::saveGame() 
{
    std::ofstream file("pliki/zapis.txt");
    if (file.is_open()) {
        file << currentLevelNumber << "\n" << currentCharacterIndex << "\n"
            << player->getHp() << "\n" << player->getScore() << "\n" // <-- ZAPISUJEMY SCORE
            << player->getPosition().x << "\n" << player->getPosition().y << "\n";
        file.close();
        if (pauseMenu) pauseMenu->setItemText(1, "ZAPISANO!");
    }
}

bool PlayState::loadGame() {
    std::ifstream file("pliki/zapis.txt");
    if (file.is_open()) {
        int lvl, charIdx, hp, score;
        float px, py;
        // CZYTAMY SCORE
        file >> lvl >> charIdx >> hp >> score >> px >> py;
        file.close();

        currentLevelNumber = lvl;
        currentCharacterIndex = charIdx;
        player->setHp(hp);
        player->setScore(score); // Ustawiamy odzyskane gwiazdki

        if (charIdx == 0) player->setColor(sf::Color::Red);
        else if (charIdx == 1) player->setColor(sf::Color::Green);
        else if (charIdx == 2) player->setColor(sf::Color::Blue);

        currentLevel.loadFromFile("pliki/level" + std::to_string(currentLevelNumber) + ".txt", mushroomTexture);
        player->setPosition(px, py);
        backgroundTexture.loadFromFile("pliki/tlo" + std::to_string(currentLevelNumber) + ".png");
        isPaused = false;

        if (pauseMenu) pauseMenu->setItemText(2, "WCZYTANO!");
        return true;
    }
    else 
    {
        if (pauseMenu) pauseMenu->setItemText(2, "BRAK ZAPISU!");
        return false;
    }
}

StateAction PlayState::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            if (!isGameOver && !isGameWon) isPaused = !isPaused;
            else return StateAction::Menu;
        }

        if (isPaused) {
            pauseMenu->handleInput(event.key.code);
            if (event.key.code == sf::Keyboard::Enter) {
                int choice = pauseMenu->getSelectedIndex();
                if (choice == 0) isPaused = false;
                else if (choice == 1) saveGame();
                else if (choice == 2) loadGame();
                else if (choice == 3) return StateAction::Menu;
            }
            return StateAction::Keep;
        }
    }

    if (!isGameOver && !isGameWon && !isPaused) player->handleEvent(event);
    return StateAction::Keep;
}

StateAction PlayState::update(sf::Time dt)
{
    // ZATRZYMANIE CZASU JEŚLI PAUZA LUB KONIEC GRY
    if (isGameOver || isGameWon || isPaused)
    {
        return StateAction::Keep;
    }

    // 1. Aktualizacja gracza i kamery
    //player->update(dt, currentLevel.getPlatforms());
    //camera.setCenter(player->getPosition());
    player->update(dt, currentLevel.getPlatforms());
    
    // --- NOWA LOGIKA KAMERY ---
    sf::Vector2f playerPos = player->getPosition();
    float maxCameraY = 300.f; // Granica, poniżej której kamera NIE spadnie (środek ekranu dla podłogi)

    // Jeśli gracz jest wysoko, kamera za nim podąża normalnie. 
    // Jeśli spada w przepaść (poniżej maxCameraY), kamera zatrzymuje się w miejscu.
    if (playerPos.y < maxCameraY)
    {
        camera.setCenter(playerPos.x, playerPos.y);
    }
    else
    {
        camera.setCenter(playerPos.x, maxCameraY);
    }
    // 2. Potworki (aktualizacja, kolizje, walka)
    auto& levelEnemies = currentLevel.getEnemies();
    for (auto& enemy : levelEnemies)
    {
        if (!enemy.isAlive()) continue;

        enemy.update(dt, currentLevel.getPlatforms());

        if (player->getGlobalBounds().intersects(enemy.getGlobalBounds()))
        {
            sf::FloatRect pBounds = player->getGlobalBounds();
            sf::FloatRect eBounds = enemy.getGlobalBounds();

            // CZY GRACZ SKOCZYŁ NA GŁOWĘ?
            if (player->getVelocity().y > 0 && pBounds.top + pBounds.height < eBounds.top + eBounds.height / 2.f)
            {
                enemy.die();
                player->bounce();

                player->incrementCombo();
                int combo = player->getCombo();

                // Przypisujemy punkty na podstawie serii (jak w Mario)
                if (combo == 1)      player->addScore(100);
                else if (combo == 2) player->addScore(200);
                else if (combo == 3) player->addScore(400);
                else if (combo == 4) player->addScore(800);
                else if (combo == 5) player->addScore(1000);
                else if (combo == 6) player->addScore(2000);
                else if (combo == 7) player->addScore(4000);
                else if (combo >= 8) 
                {
                    player->addScore(8000);
                    player->setHp(player->getHp() + 1); // 1-UP! Dodatkowe życie za combo x8 lub większe!
                    if(player->getHp() > 3) player->setHp(3); // Blokada max 3 HP, jeśli takie macie zasady
                }
            }
            else
            {
                // Jeśli nie skoczył na głowę, ale ma super moc – zabija potwora z boku i traci moc
                if (player->isSuper())
                {
                    enemy.die();
                    player->setSuper(false); // Wraca do normalnego koloru, ale nie traci HP!
                }
                else
                {
                    player->takeDamage(1); // Normalny stan -> traci 1 HP
                }
            }
        }
    }

    // Sprzątanie martwych potworów
    levelEnemies.erase(std::remove_if(levelEnemies.begin(), levelEnemies.end(), [](const Enemy& e) { return !e.isAlive(); }), levelEnemies.end());

    // 3. Gwiazdki i Punktacja
    currentLevel.removeCollectedStars(); // Usuwamy te zebrane w poprzedniej klatce

    for (auto& star : currentLevel.getStars())
    {
        if (!star.isCollected() && player->getGlobalBounds().intersects(star.getBounds()))
        {
            star.collect();
            player->addScore(200); // Dodajemy punkt za gwiazdkę!
            player->addStar();
        }
    }
    for (auto& mushroom : currentLevel.getMushrooms())
    {
        if (!mushroom.isCollected() && player->getGlobalBounds().intersects(mushroom.getBounds()))
        {
        mushroom.collect();
        player->setSuper(true); // Gracz staje się Złoty!
        player->setHp(player->getHp() + 1); // Dostaje dodatkowe żonko
        }
    }
    // Czyszczenie zebranych grzybków z wektora
    auto& lvlMushrooms = currentLevel.getMushrooms();
    lvlMushrooms.erase(std::remove_if(lvlMushrooms.begin(), lvlMushrooms.end(), [](const Mushroom& m) { return m.isCollected(); }), lvlMushrooms.end());

    // Aktualizacja napisu z punktacją (odświeżana co klatkę)
    starText.setString("Gwiazdki: " + std::to_string(player->getStarsCount()));

    // 4. Pułapki
    for (const auto& trap : currentLevel.getTraps())
    {
        if (player->getGlobalBounds().intersects(trap.getBounds()))
        {
            player->takeDamage(trap.getDamage());
        }
    }

    // 5. Portale i przechodzenie poziomów
    for (const auto& portal : currentLevel.getPortals())
    {
        if (player->getGlobalBounds().intersects(portal.getBounds()))
        {
            currentLevelNumber++;

            if (currentLevelNumber > 3)
            {
                isGameWon = true;
            }
            else
            {
                std::string nextMap = "pliki/level" + std::to_string(currentLevelNumber) + ".txt";
                if (currentLevel.loadFromFile(nextMap, mushroomTexture))
                {
                    player->setPosition(currentLevel.getPlayerSpawn().x, currentLevel.getPlayerSpawn().y);
                    std::string newBg = "pliki/tlo" + std::to_string(currentLevelNumber) + ".png";
                    backgroundTexture.loadFromFile(newBg);
                }
            }
        }
    }

    // 6. Sprawdzanie śmierci gracza
    if (!player->isAlive())
    {
        isGameOver = true;
    }
    if (!player->isAlive() || player->getPosition().y > 750.f)
    {
        isGameOver = true;
    }

    return StateAction::Keep;
}

void PlayState::render(sf::RenderWindow& window)
{
    window.setView(window.getDefaultView());
    window.draw(backgroundSprite);
    window.setView(camera);
    currentLevel.render(window);
    for (auto& p : currentLevel.getPortals()) p.render(window);
    for (auto& e : currentLevel.getEnemies()) e.render(window);
    player->render(window);

    window.setView(window.getDefaultView());
    for (int i = 0; i < 3; i++) {
        heartShape.setPosition(20.f + (i * 50.f), 20.f);
        heartShape.setFillColor(i < player->getHp() ? sf::Color::Red : sf::Color(60, 60, 60));
        window.draw(heartShape);
    }
    // --- 2. TUTAJ COŚ NOWEGO: RYSOWANIE SCORE NA ŚRODKU ---
    scoreText.setString("Score: " + std::to_string(player->getScore()));
    // Ustawiamy na środku ekranu (400) minus połowa szerokości tekstu, wysokość 25.f
    scoreText.setPosition(300.f - scoreText.getGlobalBounds().width / 2.f, 25.f);
    window.draw(scoreText);

    if (isPaused) pauseMenu->render(window);
    if (isGameOver) 
    {
        gameOverText.setPosition(400.f - gameOverText.getGlobalBounds().width / 2.f, 240.f);
        resetText.setPosition(400.f - resetText.getGlobalBounds().width / 2.f, 320.f);
        window.draw(gameOverText); 
        window.draw(resetText);
        //gameOverText.setPosition(camera.getCenter().x - (gameOverText.getGlobalBounds().width / 2.f), camera.getCenter().y - 40.f);
        //resetText.setPosition(camera.getCenter().x - (resetText.getGlobalBounds().width / 2.f), camera.getCenter().y + 30.f);
        //window.draw(gameOverText);
        //window.draw(resetText);
    }
    else if (isGameWon) 
    {
        gameWonText.setPosition(400.f - gameWonText.getGlobalBounds().width / 2.f, 240.f);
        resetText.setPosition(400.f - resetText.getGlobalBounds().width / 2.f, 320.f);
        window.draw(gameWonText); window.draw(resetText);

        //gameWonText.setPosition(camera.getCenter().x - (gameWonText.getGlobalBounds().width / 2.f), camera.getCenter().y - 40.f);
        //resetText.setPosition(camera.getCenter().x - (resetText.getGlobalBounds().width / 2.f), camera.getCenter().y + 30.f);
        
        //window.draw(gameWonText);
        //window.draw(resetText);
    }
    window.draw(starText);
}
