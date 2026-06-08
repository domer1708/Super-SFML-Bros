#include "State.h"
#include "Enemy.h" // <-- PAMIĘTAJ: Musimy dołączyć nasz nowy plik!

PlayState::PlayState(int characterIndex)
{
    player = std::make_unique<Player>();
    camera.setSize(800.f, 600.f);
    
    isGameOver = false;

    // Ładujemy czcionkę do napisów końcowych
    font.loadFromFile("pliki/arial.ttf"); 
    
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(60);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);

    resetText.setFont(font);
    resetText.setString("Wcisnij ESC, aby wrocic do menu");
    resetText.setCharacterSize(20);
    resetText.setFillColor(sf::Color::White);

    // --- USTAWIANIE KOLORU POSTACI ---
    if (characterIndex == 0) {
        player->setColor(sf::Color::Red);   // Mario
    }
    else if (characterIndex == 1) {
        player->setColor(sf::Color::Green); // Luigi
    }
    else if (characterIndex == 2) {
        player->setColor(sf::Color::Blue);  // Toad
    }
    
    // --- ŁADOWANIE POZIOMU ---
    if (currentLevel.loadFromFile("pliki/level1.txt"))
    {
        player->setPosition(currentLevel.getPlayerSpawn().x, currentLevel.getPlayerSpawn().y);
    }

    // --- TWORZENIE 4 POTWORKÓW W STAŁYCH MIEJSCACH ---
    // (Podaję przykładowe pozycje X, Y - zmieńcie je pod Wasz układ platform!)
    enemies.push_back(Enemy(300.f, 200.f));
    enemies.push_back(Enemy(600.f, 300.f));
    enemies.push_back(Enemy(900.f, 200.f));
    enemies.push_back(Enemy(1200.f, 400.f));
}

StateAction PlayState::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
    {
        return StateAction::Menu; // ESC zawsze wraca do menu
    }

    // Jeśli gra się skończyła, ignorujemy ruchy gracza
    if (!isGameOver)
    {
        player->handleEvent(event);
    }
    
    return StateAction::Keep;
}

StateAction PlayState::update(sf::Time dt)
{
    // Jeśli jest Game Over, zatrzymujemy całą fizykę gry
    if (isGameOver)
    {
        // Ustawiamy napisy na środku aktualnego widoku kamery, żeby gracz je widział
        sf::Vector2f camCenter = camera.getCenter();
        
        gameOverText.setPosition(camCenter.x - gameOverText.getGlobalBounds().width / 2.f, camCenter.y - 60.f);
        resetText.setPosition(camCenter.x - resetText.getGlobalBounds().width / 2.f, camCenter.y + 20.f);
        
        return StateAction::Keep;
    }

    // 1. Aktualizacja gracza
    player->update(dt, currentLevel.getPlatforms()); 
    camera.setCenter(player->getPosition()); // kamera podąża za graczem

    // 2. Aktualizacja wszystkich potworków + sprawdzanie kolizji z graczem
    for (auto& enemy : enemies)
    {
        enemy.update(dt, currentLevel.getPlatforms());

        // Sprawdzamy, czy gracz dotknął różowego kwadratu
        if (player->getGlobalBounds().intersects(enemy.getGlobalBounds()))
        {
            isGameOver = true; // ŁUP! Przegrana
        }
    }

    return StateAction::Keep;
}

void PlayState::render(sf::RenderWindow& window)
{
    window.setView(camera);
    
    // Rysujemy świat gry
    currentLevel.render(window);
    
    // Rysujemy potworki
    for (auto& enemy : enemies)
    {
        enemy.render(window);
    }
    
    // Rysujemy gracza
    player->render(window);

    // Jeśli przegraliśmy, nakładamy napisy na sam wierzch screena
    if (isGameOver)
    {
        window.draw(gameOverText);
        window.draw(resetText);
    }
}