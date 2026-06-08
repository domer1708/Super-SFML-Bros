#include "State.h"
#include "Enemy.h"
#include "Elements.h"
#include <algorithm>

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

    heartShape.setPointCount(6);
    heartShape.setPoint(0, sf::Vector2f(20.f, 10.f));  // Górne wcięcie
    heartShape.setPoint(1, sf::Vector2f(30.f, 0.f));   // Prawy łuk
    heartShape.setPoint(2, sf::Vector2f(40.f, 12.f));  // Prawa krawędź
    heartShape.setPoint(3, sf::Vector2f(20.f, 38.f));  // Dolny szpic
    heartShape.setPoint(4, sf::Vector2f(0.f, 12.f));   // Lewa krawędź
    heartShape.setPoint(5, sf::Vector2f(10.f, 0.f));   // Lewy łuk
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
        return StateAction::Keep;
    }

    // 1. Aktualizacja gracza i kamery
    player->update(dt, currentLevel.getPlatforms());
    camera.setCenter(player->getPosition());


    // ==========================================
    // TUTAJ WKLEJASZ TEN NOWY KOD (ZASTĘPUJE STARE WROGI):
    // ==========================================
    for (auto& enemy : enemies)
    {
        if (!enemy.isAlive()) continue; // Ignoruj zabite potwory

        enemy.update(dt, currentLevel.getPlatforms());

        // Sprawdzamy kolizję
        if (player->getGlobalBounds().intersects(enemy.getGlobalBounds()))
        {
            sf::FloatRect pBounds = player->getGlobalBounds();
            sf::FloatRect eBounds = enemy.getGlobalBounds();

            // CZY GRACZ SKOCZYŁ NA GŁOWĘ?
            if (player->getVelocity().y > 0 && pBounds.top + pBounds.height < eBounds.top + eBounds.height / 2.f)
            {
                enemy.die();          // Zabijamy wroga
                player->bounce();     // Gracz odskakuje do góry
            }
            else
            {
                player->takeDamage(1); // Uderzenie w bok = strata 1 życia
            }
        }
    }

    // SPRZĄTANIE: Usuwamy zabite potworki z wektora
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) { return !e.isAlive(); }), enemies.end());

    // 3. Gwiazdki i Pułapki (Zostawiasz to, co miałeś)
    for (auto& star : currentLevel.getStars())
    {
        if (!star.isCollected() && player->getGlobalBounds().intersects(star.getBounds()))
        {
            star.collect();
        }
    }
    currentLevel.removeCollectedStars();

    for (const auto& trap : currentLevel.getTraps())
    {
        if (player->getGlobalBounds().intersects(trap.getBounds()))
        {
            player->takeDamage(trap.getDamage());
        }
    }

    // 4. Sprawdzanie śmierci gracza (strata wszystkich 3 żyć)
    if (!player->isAlive())
    {
        isGameOver = true;
    }

    return StateAction::Keep;
}

void PlayState::render(sf::RenderWindow& window)
{
    // ==========================================
    // KROK 1: RYSOWANIE ŚWIATA GRY (Kamera podąża za graczem)
    // ==========================================
    window.setView(camera);

    currentLevel.render(window); // Rysujemy mapę

    for (auto& enemy : enemies)  // Rysujemy potworki
    {
        enemy.render(window);
    }

    player->render(window);      // Rysujemy gracza

    // ==========================================
    // KROK 2: RYSOWANIE INTERFEJSU (HUD - Ekran stoi w miejscu)
    // ==========================================
    window.setView(window.getDefaultView()); // Przełączamy na widok statyczny okna!

    // Rysujemy 3 serduszka obok siebie
    for (int i = 0; i < 3; i++)
    {
        // Wyliczamy pozycję dla każdego serca (odstęp co 50 pikseli w prawo)
        heartShape.setPosition(20.f + (i * 50.f), 20.f);

        // MAGIA: Jeśli indeks pętli jest mniejszy niż obecne życie gracza,
        // serce jest czerwone. W przeciwnym wypadku staje się ciemnoszare (zgaszone)!
        if (i < player->getHp())
        {
            heartShape.setFillColor(sf::Color::Red);
        }
        else
        {
            heartShape.setFillColor(sf::Color(60, 60, 60)); // Ciemnoszary
        }

        window.draw(heartShape);
    }

    // Jeśli przegraliśmy, nakładamy napisy Game Over na sam wierzch statycznego ekranu
    if (isGameOver)
    {
        // Ponieważ widok jest teraz statyczny (800x600), możemy wyśrodkować napisy na sztywno
        gameOverText.setPosition(400.f - gameOverText.getGlobalBounds().width / 2.f, 240.f);
        resetText.setPosition(400.f - resetText.getGlobalBounds().width / 2.f, 320.f);

        window.draw(gameOverText);
        window.draw(resetText);
    }
}