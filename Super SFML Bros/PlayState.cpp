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

    font.loadFromFile("pliki/arial.ttf");

    // Konfiguracja Napisów (Game Over itp.)
    gameOverText.setFont(font); gameOverText.setString("GAME OVER"); gameOverText.setCharacterSize(60); gameOverText.setFillColor(sf::Color::Red); gameOverText.setStyle(sf::Text::Bold);
    gameWonText.setFont(font); gameWonText.setString("WYGRALES GRE!"); gameWonText.setCharacterSize(60); gameWonText.setFillColor(sf::Color::Yellow); gameWonText.setStyle(sf::Text::Bold);
    resetText.setFont(font); resetText.setString("Wcisnij ESC, aby wrocic do menu"); resetText.setCharacterSize(20); resetText.setFillColor(sf::Color::White);

    // --- HUD GWIAZDEK ---
    starText.setFont(font);
    starText.setCharacterSize(24);
    starText.setFillColor(sf::Color::Yellow);
    starText.setPosition(620.f, 20.f); // Prawy górny róg
    starText.setString("Gwiazdki: 0");

    // Próba wczytania z menu głównego
    bool loaded = false;
    if (loadFromSave) {
        loaded = loadGame();
    }

    // Jeśli nowa gra, lub brak zapisu (fallback)
    if (!loaded) {
        if (characterIndex == 0) player->setColor(sf::Color::Red);
        else if (characterIndex == 1) player->setColor(sf::Color::Green);
        else if (characterIndex == 2) player->setColor(sf::Color::Blue);

        currentLevel.loadFromFile("pliki/level1.txt");
        player->setPosition(currentLevel.getPlayerSpawn().x, currentLevel.getPlayerSpawn().y);
    }

    // Geometria serca
    heartShape.setPointCount(6); heartShape.setPoint(0, sf::Vector2f(20.f, 10.f)); heartShape.setPoint(1, sf::Vector2f(30.f, 0.f)); heartShape.setPoint(2, sf::Vector2f(40.f, 12.f)); heartShape.setPoint(3, sf::Vector2f(20.f, 38.f)); heartShape.setPoint(4, sf::Vector2f(0.f, 12.f)); heartShape.setPoint(5, sf::Vector2f(10.f, 0.f));

    // Menu pauzy
    pauseMenu = std::make_unique<PauseMenu>(font);
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

        currentLevel.loadFromFile("pliki/level" + std::to_string(currentLevelNumber) + ".txt");
        player->setPosition(px, py);
        isPaused = false;

        if (pauseMenu) pauseMenu->setItemText(2, "WCZYTANO!");
        return true;
    }
    else {
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
    player->update(dt, currentLevel.getPlatforms());
    camera.setCenter(player->getPosition());

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
            }
            else
            {
                player->takeDamage(1);
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
            player->addScore(1); // Dodajemy punkt za gwiazdkę!
        }
    }

    // Aktualizacja napisu z punktacją (odświeżana co klatkę)
    starText.setString("Gwiazdki: " + std::to_string(player->getScore()));

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
                if (currentLevel.loadFromFile(nextMap))
                {
                    player->setPosition(currentLevel.getPlayerSpawn().x, currentLevel.getPlayerSpawn().y);
                }
            }
        }
    }

    // 6. Sprawdzanie śmierci gracza
    if (!player->isAlive())
    {
        isGameOver = true;
    }

    return StateAction::Keep;
}

void PlayState::render(sf::RenderWindow& window)
{
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

    if (isPaused) pauseMenu->render(window);
    if (isGameOver) {
        gameOverText.setPosition(400.f - gameOverText.getGlobalBounds().width / 2.f, 240.f);
        resetText.setPosition(400.f - resetText.getGlobalBounds().width / 2.f, 320.f);
        window.draw(gameOverText); window.draw(resetText);
    }
    else if (isGameWon) {
        gameWonText.setPosition(400.f - gameWonText.getGlobalBounds().width / 2.f, 240.f);
        resetText.setPosition(400.f - resetText.getGlobalBounds().width / 2.f, 320.f);
        window.draw(gameWonText); window.draw(resetText);
    }
    window.draw(starText);
}