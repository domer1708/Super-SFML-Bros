#include "State.h"

PlayState::PlayState(int characterIndex)
{
	player = std::make_unique<Player>();
	camera.setSize(800.f, 600.f);
    
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
    // ---
    if (currentLevel.loadFromFile("pliki/level1.txt"))
    {
        player->setPosition(currentLevel.getPlayerSpawn().x, currentLevel.getPlayerSpawn().y);
    }
}

StateAction PlayState::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
    {
        return StateAction::Menu; // esc = menu
    }

    player->handleEvent(event); // jeśli nie gramy dalej
    return StateAction::Keep;
}

StateAction PlayState::update(sf::Time dt)
{
    player->update(dt, currentLevel.getPlatforms()); // przelicza fizyke gracza
    camera.setCenter(player->getPosition()); // kamera na gracza
    return StateAction::Keep;
}

void PlayState::render(sf::RenderWindow& window)
{
    window.setView(camera);
    currentLevel.render(window);
    player->render(window);
}