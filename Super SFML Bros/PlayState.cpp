#include "State.h"
#include "Enemy.h"
#include "Elements.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>

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

    // Konfiguracja Napisów
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
    starText.setPosition(420.f, 20.f);
    starText.setString("Gwiazdki: 0");

    // --- HUD PUNKTÓW (SCORE) ---
    scoreText.setFont(font);
    scoreText.setCharacterSize(16);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString("Score: 000000");

    // =======================================================
    // --- NAPRAWA BIAŁEGO TŁA W TEKSTURACH ---
    // =======================================================
    sf::Image mushroomImage;
    if (mushroomImage.loadFromFile("pliki/masrums.png"))
    {
        // Wymaż kolor biały (zrób przezroczysty)
        mushroomImage.createMaskFromColor(sf::Color::White);
        mushroomTexture.loadFromImage(mushroomImage);
    }
    else
    {
        std::cout << "Blad ladowania pliku pliki/masrums.png!" << std::endl;
    }

    sf::Image groundImage;
    if (groundImage.loadFromFile("pliki/ground.png"))
    {
        // Wymaż kolor biały z ziemi
        groundImage.createMaskFromColor(sf::Color::White);
        groundTexture.loadFromImage(groundImage);
        groundTexture.setRepeated(true);
    }
    else
    {
        std::cout << "Blad ladowania pliku pliki/ground.png!" << std::endl;
    }
    // =======================================================

    // Dźwięk zabijania potwora
    if (stompBuffer.loadFromFile("pliki/stomp.ogg"))
    {
        stompSound.setBuffer(stompBuffer);
        stompSound.setVolume(75.f);
    }
    else
    {
        std::cout << "Blad: Nie udalo sie wczytac pliki/stomp.ogg!" << std::endl;
    }

    // Dźwięk zjadania grzybów 
    if (powerupBuffer.loadFromFile("pliki/powerup.ogg"))
    {
        powerupSound.setBuffer(powerupBuffer);
        powerupSound.setVolume(60.f);
    }
    else
    {
        std::cout << "Blad: Nie udalo sie wczytac pliki/powerup.ogg!" << std::endl;
    }

    keyText.setFont(font);
    keyText.setCharacterSize(16);
    keyText.setFillColor(sf::Color(255, 215, 0));
    keyText.setPosition(650.f, 60.f);

    bool loaded = false;
    if (loadFromSave)
    {
        loaded = loadGame();
    }

    if (!loaded) {
        if (characterIndex == 0) player->setColor(sf::Color::Red);
        else if (characterIndex == 1) player->setColor(sf::Color::Green);
        else if (characterIndex == 2) player->setColor(sf::Color::Blue);

        currentLevel.loadFromFile("pliki/level1.txt", mushroomTexture, groundTexture);
        player->setPosition(currentLevel.getPlayerSpawn().x, currentLevel.getPlayerSpawn().y);
    }

    heartShape.setPointCount(6); heartShape.setPoint(0, sf::Vector2f(20.f, 10.f)); heartShape.setPoint(1, sf::Vector2f(30.f, 0.f)); heartShape.setPoint(2, sf::Vector2f(40.f, 12.f)); heartShape.setPoint(3, sf::Vector2f(20.f, 38.f)); heartShape.setPoint(4, sf::Vector2f(0.f, 12.f)); heartShape.setPoint(5, sf::Vector2f(10.f, 0.f));

    pauseMenu = std::make_unique<PauseMenu>(font);

    std::string bgName = "pliki/tlo" + std::to_string(currentLevelNumber) + ".png";
    if (!backgroundTexture.loadFromFile(bgName))
    {
        std::cout << "Blad ladowania pliku " << bgName << "!" << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture);

    float scaleX = 800.f / backgroundTexture.getSize().x;
    float scaleY = 600.f / backgroundTexture.getSize().y;
    backgroundSprite.setScale(scaleX, scaleY);
}

void PlayState::saveGame() {
    std::ofstream file("pliki/zapis.txt");
    if (file.is_open()) {
        int bossHp = 0;
        if (!currentLevel.getBosses().empty()) {
            bossHp = currentLevel.getBosses()[0].getHp();
        }

        file << currentLevelNumber << "\n" << currentCharacterIndex << "\n"
            << player->getHp() << "\n" << player->getScore() << "\n"
            << player->getStarsCount() << "\n" << player->hasKey() << "\n"
            << player->hasCheckpoint() << "\n"
            << player->getCheckpointPos().x << "\n" << player->getCheckpointPos().y << "\n"
            << player->getPosition().x << "\n" << player->getPosition().y << "\n"
            << bossHp << "\n";

        file.close();
        if (pauseMenu) pauseMenu->setItemText(1, "ZAPISANO!");
    }
}

bool PlayState::loadGame() {
    std::ifstream file("pliki/zapis.txt");
    if (file.is_open()) {
        int lvl, charIdx, hp, score, stars, bossHp;
        bool hasKey, hasCheck;
        float cx, cy, px, py;

        file >> lvl >> charIdx >> hp >> score >> stars >> hasKey >> hasCheck >> cx >> cy >> px >> py >> bossHp;
        file.close();

        currentLevelNumber = lvl; currentCharacterIndex = charIdx;
        player->setHp(hp); player->setScore(score); player->setStarsCount(stars); player->setKey(hasKey);

        if (hasCheck) player->setCheckpoint(sf::Vector2f(cx, cy));
        else player->resetCheckpoint();

        if (charIdx == 0) player->setColor(sf::Color::Red);
        else if (charIdx == 1) player->setColor(sf::Color::Green);
        else if (charIdx == 2) player->setColor(sf::Color::Blue);

        // Ładujemy mapę od nowa
        currentLevel.loadFromFile("pliki/level" + std::to_string(currentLevelNumber) + ".txt", mushroomTexture, groundTexture);

        // --- POPRAWKA: WIZUALNA REAKTYWACJA CHECKPOINTU PO WCZYTANIU ---
        if (hasCheck) {
            for (auto& c : currentLevel.getCheckpoints()) {
                // Jeśli pozycja checkpointu na mapie zgadza się z wczytaną pozycją cx
                if (std::abs(c.getBounds().left - cx) < 10.f) {
                    c.activate(); // Zaświeć flagę na niebiesko!
                }
            }
        }
        // ---------------------------------------------------------------

        // Stan Bossa
        if (!currentLevel.getBosses().empty()) {
            if (bossHp <= 0) currentLevel.getBosses().clear();
            else currentLevel.getBosses()[0].setHp(bossHp);
        }

        player->setPosition(px, py);
        backgroundTexture.loadFromFile("pliki/tlo" + std::to_string(currentLevelNumber) + ".png");
        isPaused = false;
        return true;
    }
    return false;
}

StateAction PlayState::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            if (!isGameOver && !isGameWon) {
                isPaused = !isPaused;
                if (isPaused) {
                    pauseMenu->setItemText(1, "Zapisz Gre");
                }
            }
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
    if (isGameOver || isGameWon || isPaused) return StateAction::Keep;

    // 0. Aktualizacja wewnętrzna poziomu
    currentLevel.updateLevelElements(dt);

    std::vector<sf::RectangleShape> solidBlocks = currentLevel.getPlatforms();
    for (const auto& vp : currentLevel.getVanishingPlatforms()) {
        if (vp.isSolid()) solidBlocks.push_back(vp.getShape());
    }
    for (const auto& mp : currentLevel.getMovingPlatforms()) {
        solidBlocks.push_back(mp.getShape());
    }
    for (const auto& ice : currentLevel.getIceBlocks()) solidBlocks.push_back(ice);
    for (const auto& v : currentLevel.getElevators()) solidBlocks.push_back(v.getShape());

    // --- SYSTEM ŚLISKIEJ PODŁOGI (LÓD) ---
    sf::FloatRect iceFeet = player->getGlobalBounds();
    iceFeet.top += 2.f;
    bool onIce = false;
    for (const auto& ice : currentLevel.getIceBlocks()) {
        if (iceFeet.intersects(ice.getGlobalBounds())) {
            onIce = true;
            break;
        }
    }
    if (onIce) player->setFriction(0.5f);
    else player->setFriction(8.f);


    // =====================================================================
    // --- PRZYKLEJANIE DO PLATFORM ---
    // =====================================================================
    sf::FloatRect pBounds = player->getGlobalBounds();
    bool onMovingPlatform = false;

    for (const auto& mp : currentLevel.getMovingPlatforms()) {
        sf::FloatRect mBounds = mp.getBounds();
        if (pBounds.left + pBounds.width - 4.f > mBounds.left && pBounds.left + 4.f < mBounds.left + mBounds.width) {
            if (std::abs((pBounds.top + pBounds.height) - mBounds.top) < 20.f && player->getVelocity().y >= 0.f) {
                player->setPosition(player->getPosition().x + mp.getDeltaMovement().x, mBounds.top - pBounds.height - 0.1f);
                onMovingPlatform = true;
                break;
            }
        }
    }

    pBounds = player->getGlobalBounds();
    for (const auto& v : currentLevel.getElevators()) {
        sf::FloatRect vBounds = v.getBounds();
        if (pBounds.left + pBounds.width - 4.f > vBounds.left && pBounds.left + 4.f < vBounds.left + vBounds.width) {
            float oldElevatorTop = vBounds.top - v.getDeltaMovement().y;
            float playerFootY = pBounds.top + pBounds.height;
            if (std::abs(playerFootY - oldElevatorTop) < 30.f && player->getVelocity().y >= 0.f) {
                player->setPosition(player->getPosition().x, vBounds.top - pBounds.height - 0.1f);
                onMovingPlatform = true;
                break;
            }
        }
    }
    // =====================================================================

    player->update(dt, solidBlocks);

    if (onMovingPlatform) {
        player->resetJumping();
    }

    // --- ZNIKAJĄCE PLATFORMY ---
    // (Zmieniono nazwę na vanishFeet, by uniknąć problemu z kompilacją!)
    sf::FloatRect vanishFeet = player->getGlobalBounds();
    vanishFeet.height += 2.f;

    for (auto& vp : currentLevel.getVanishingPlatforms()) {
        if (vp.isSolid() && vanishFeet.intersects(vp.getBounds())) {
            if (player->getVelocity().y >= 0) vp.trigger();
        }
    }

    // --- CHECKPOINTY ---
    for (auto& c : currentLevel.getCheckpoints()) {
        if (!c.isActivated() && player->getGlobalBounds().intersects(c.getBounds())) {
            c.activate();
            player->setCheckpoint(sf::Vector2f(c.getBounds().left, c.getBounds().top - 10.f));
        }
    }

    // --- WIEŻYCZKI ---
    for (auto& b : currentLevel.getBullets()) {
        if (b.isAlive() && player->getGlobalBounds().intersects(b.getBounds())) {
            b.destroy();
            player->takeDamage(1);
        }
    }

    // --- KAMERA ---
    sf::Vector2f playerPos = player->getPosition();
    float maxCameraY = 300.f;
    if (playerPos.y < maxCameraY) camera.setCenter(playerPos.x, playerPos.y);
    else camera.setCenter(playerPos.x, maxCameraY);

    // --- POTWORY ---
    auto& levelEnemies = currentLevel.getEnemies();
    for (auto& enemy : levelEnemies) {
        if (!enemy.isAlive()) continue;
        enemy.update(dt, currentLevel.getPlatforms());
        if (player->getGlobalBounds().intersects(enemy.getGlobalBounds())) {
            if (player->getVelocity().y > 0 && player->getGlobalBounds().top + player->getGlobalBounds().height < enemy.getGlobalBounds().top + enemy.getGlobalBounds().height / 2.f) {
                enemy.die();
                stompSound.play();
                player->bounce();
                player->incrementCombo();
                int combo = player->getCombo();
                if (combo == 1)      player->addScore(100);
                else if (combo == 2) player->addScore(200);
                else if (combo >= 3) player->addScore(500);
            }
            else {
                if (player->isSuper()) { enemy.die(); player->setSuper(false); }
                else player->takeDamage(1);
            }
        }
    }
    levelEnemies.erase(std::remove_if(levelEnemies.begin(), levelEnemies.end(), [](const Enemy& e) { return !e.isAlive(); }), levelEnemies.end());

    // --- BOSS ---
    auto& levelBosses = currentLevel.getBosses();
    for (auto& boss : levelBosses) {
        if (!boss.isAlive()) continue;
        boss.updateBoss(dt, solidBlocks, player->getPosition());

        if (player->getGlobalBounds().intersects(boss.getGlobalBounds())) {
            if (player->getVelocity().y > 0 && player->getGlobalBounds().top + player->getGlobalBounds().height < boss.getGlobalBounds().top + 20.f) {
                boss.takeDamage();
                stompSound.play();
                player->bounce();
                player->addScore(300);
                if (!boss.isAlive()) {
                    player->addScore(2000);
                    currentLevel.getKeys().push_back(Key(boss.getGlobalBounds().left, boss.getGlobalBounds().top));
                }
            }
            else {
                player->takeDamage(1);
            }
        }
    }
    levelBosses.erase(std::remove_if(levelBosses.begin(), levelBosses.end(), [](const Boss& b) { return !b.isAlive(); }), levelBosses.end());

    // --- ZNAJDŹKI ---
    currentLevel.removeCollectedStars();
    for (auto& star : currentLevel.getStars()) {
        if (!star.isCollected() && player->getGlobalBounds().intersects(star.getBounds())) {
            star.collect(); player->addScore(200); player->addStar();
        }
    }
    starText.setString("Gwiazdki: " + std::to_string(player->getStarsCount()));

    for (auto& mushroom : currentLevel.getMushrooms()) {
        if (!mushroom.isCollected() && player->getGlobalBounds().intersects(mushroom.getBounds()))
        {
            mushroom.collect();
            powerupSound.play();
            player->setSuper(true);
            player->setHp(player->getHp() + 1);
            if (player->getHp() > 3) player->setHp(3);
        }
    }
    currentLevel.getMushrooms().erase(std::remove_if(currentLevel.getMushrooms().begin(), currentLevel.getMushrooms().end(), [](const Mushroom& m) { return m.isCollected(); }), currentLevel.getMushrooms().end());

    for (const auto& trap : currentLevel.getTraps()) {
        if (player->getGlobalBounds().intersects(trap.getBounds())) player->takeDamage(trap.getDamage());
    }

    for (const auto& tramp : currentLevel.getTrampolines()) {
        if (player->getGlobalBounds().intersects(tramp.getBounds()) && player->getVelocity().y > 0) {
            player->superBounce();
        }
    }

    // --- KLUCZE I PORTAL ---
    currentLevel.removeCollectedKeys();
    for (auto& key : currentLevel.getKeys()) {
        if (!key.isCollected() && player->getGlobalBounds().intersects(key.getBounds())) {
            key.collect(); player->collectKey(); player->addScore(100);
        }
    }
    for (auto& portal : currentLevel.getPortals()) {
        if (player->getGlobalBounds().intersects(portal.getBounds())) {
            if (portal.isLocked()) {
                if (player->hasKey()) { portal.unlock(); player->useKey(); player->addScore(500); }
            }
            else {
                currentLevelNumber++;
                if (currentLevelNumber > 3) isGameWon = true;
                else {
                    std::string nextMap = "pliki/level" + std::to_string(currentLevelNumber) + ".txt";

                    if (currentLevel.loadFromFile(nextMap, mushroomTexture, groundTexture)) {
                        player->setPosition(currentLevel.getPlayerSpawn().x, currentLevel.getPlayerSpawn().y);
                        player->resetCheckpoint();
                        backgroundTexture.loadFromFile("pliki/tlo" + std::to_string(currentLevelNumber) + ".png");
                    }
                }
            }
        }
    }

    if (!player->isAlive() || player->getPosition().y > 750.f)
    {
        if (player->hasCheckpoint())
        {
            player->setHp(3);
            player->resetVelocity();

            currentLevel.loadFromFile("pliki/level" + std::to_string(currentLevelNumber) + ".txt", mushroomTexture, groundTexture);
            player->setPosition(player->getCheckpointPos().x, player->getCheckpointPos().y);

            for (auto& c : currentLevel.getCheckpoints()) {
                if (std::abs(c.getBounds().left - player->getCheckpointPos().x) < 10.f) {
                    c.activate();
                }
            }
        }
        else
        {
            isGameOver = true;
        }
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

    scoreText.setString("Score: " + std::to_string(player->getScore()));
    scoreText.setPosition(300.f - scoreText.getGlobalBounds().width / 2.f, 25.f);
    window.draw(scoreText);

    if (isPaused) pauseMenu->render(window);
    if (isGameOver)
    {
        gameOverText.setPosition(400.f - gameOverText.getGlobalBounds().width / 2.f, 240.f);
        resetText.setPosition(400.f - resetText.getGlobalBounds().width / 2.f, 320.f);
        window.draw(gameOverText);
        window.draw(resetText);
    }
    else if (isGameWon)
    {
        gameWonText.setPosition(400.f - gameWonText.getGlobalBounds().width / 2.f, 240.f);
        resetText.setPosition(400.f - resetText.getGlobalBounds().width / 2.f, 320.f);
        window.draw(gameWonText); window.draw(resetText);
    }
    window.draw(starText);
    keyText.setString(player->hasKey() ? "Klucz: TAK" : "Klucz: NIE");
    window.draw(keyText);
}