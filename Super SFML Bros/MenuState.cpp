#include "State.h"
#include <cstdlib> // do losowania pozycji gwiazd

MenuState::MenuState()
{
    font.loadFromFile("pliki/PressStart2P-Regular.ttf");

    title.setFont(font);
    title.setString("SUPER SFML BROS");
    title.setCharacterSize(42);
    title.setFillColor(sf::Color::White);
    float titleWidth = title.getGlobalBounds().width;
    title.setPosition(sf::Vector2f((800.f - titleWidth) / 2.f, 60.f));

    menuFrame.setSize(sf::Vector2f(340.f, 250.f));
    menuFrame.setPosition(sf::Vector2f((800.f - 340.f) / 2.f, 200.f));
    menuFrame.setFillColor(sf::Color(20, 20, 20, 220));
    menuFrame.setOutlineThickness(5.f);
    menuFrame.setOutlineColor(sf::Color::Yellow);

    // --- 3 OPCJE MENU ---
    std::string options[3] = { "Nowa Gra", "Wczytaj Gre", "Wyjdz" };
    for (int i = 0; i < 3; i++) {
        menu[i].setFont(font);
        menu[i].setFillColor(i == 0 ? sf::Color::Yellow : sf::Color::White);
        menu[i].setString(options[i]);
        menu[i].setCharacterSize(24);
        float wWidth = menu[i].getGlobalBounds().width;

        // IDEALNE WYŚRODKOWANIE PIONOWE:
        // Startujemy od Y=250 (nie 230) i dajemy odstępy po 60px (nie 50)
        menu[i].setPosition(sf::Vector2f((800.f - wWidth) / 2.f, 250.f + (i * 60.f)));
    }

    selectedItemIndex = 0;
    isChoosingCharacter = false;
    selectedCharacterIndex = 0;

    // --- MENU WYBORU POSTACI (TEŻ WYŚRODKOWANE) ---
    float startX = 265.f;
    for (int i = 0; i < 3; i++) {
        characterBoxes[i].setSize(sf::Vector2f(65.f, 100.f));
        // Opuszczone na Y=250 (wcześniej 240)
        characterBoxes[i].setPosition(sf::Vector2f(startX + (i * 75.f), 250.f));
    }
    characterBoxes[0].setFillColor(sf::Color::Red);
    characterBoxes[1].setFillColor(sf::Color::Green);
    characterBoxes[2].setFillColor(sf::Color::Blue);

    characterNames[0].setString("Mario");
    characterNames[1].setString("Luigi");
    characterNames[2].setString("Toad");

    for (int i = 0; i < 3; i++) {
        characterNames[i].setFont(font);
        characterNames[i].setCharacterSize(14);
        characterNames[i].setFillColor(i == 0 ? sf::Color::Yellow : sf::Color::White);
        float nWidth = characterNames[i].getGlobalBounds().width;
        float boxCenter = characterBoxes[i].getPosition().x + 32.5f;
        // Opuszczone na Y=370 (wcześniej 360)
        characterNames[i].setPosition(sf::Vector2f(boxCenter - (nWidth / 2.f), 370.f));
    }

    initStars();
}

void MenuState::initStars()
{
    for (int i = 0; i < 60; i++) // Tworzymy 60 gwiazd
    {
        Star star;
        star.position.x = static_cast<float>(rand() % 800);
        star.position.y = static_cast<float>(rand() % 600);
        star.speed = static_cast<float>((rand() % 150) + 50); // Prędkość od 50 do 200 px/s
        star.size = static_cast<float>((rand() % 3) + 1);     // Wielkość od 1 do 3 pikseli
        stars.push_back(star);
    }
}

StateAction MenuState::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (isChoosingCharacter)
        {
            if ((event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) && selectedCharacterIndex > 0)
            {
                characterNames[selectedCharacterIndex].setFillColor(sf::Color::White);
                characterBoxes[selectedCharacterIndex].setOutlineThickness(0.f);
                selectedCharacterIndex--;
                characterNames[selectedCharacterIndex].setFillColor(sf::Color::Yellow);
            }
            else if ((event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) && selectedCharacterIndex < 2)
            {
                characterNames[selectedCharacterIndex].setFillColor(sf::Color::White);
                characterBoxes[selectedCharacterIndex].setOutlineThickness(0.f);
                selectedCharacterIndex++;
                characterNames[selectedCharacterIndex].setFillColor(sf::Color::Yellow);
            }
            else if (event.key.code == sf::Keyboard::Escape) isChoosingCharacter = false;
            else if (event.key.code == sf::Keyboard::Enter) return StateAction::Play;
        }
        else
        {
            if ((event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) && selectedItemIndex > 0)
            {
                menu[selectedItemIndex].setFillColor(sf::Color::White);
                selectedItemIndex--;
                menu[selectedItemIndex].setFillColor(sf::Color::Yellow);
            }
            // ZMIANA: Zezwalamy zjechać na index mniejszy niż 2 (czyli 0, 1 i 2)
            else if ((event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) && selectedItemIndex < 2)
            {
                menu[selectedItemIndex].setFillColor(sf::Color::White);
                selectedItemIndex++;
                menu[selectedItemIndex].setFillColor(sf::Color::Yellow);
            }
            else if (event.key.code == sf::Keyboard::Enter)
            {
                if (selectedItemIndex == 0) isChoosingCharacter = true;
                else if (selectedItemIndex == 1) return StateAction::Load; // Wczytaj
                else if (selectedItemIndex == 2) return StateAction::Exit; // Wyjdź
            }
        }
    }
    return StateAction::Keep;
}

StateAction MenuState::update(sf::Time dt)
{
    // --- AKTUALIZACJA RUCHU GWIAZD ---
    for (auto& star : stars)
    {
        star.position.y += star.speed * dt.asSeconds(); // Gwiazdy lecą w dół

        // Jeśli gwiazda wyleci za dolny ekran, przenosimy ją na górę w losowe miejsce X
        if (star.position.y > 600.f)
        {
            star.position.y = 0.f;
            star.position.x = static_cast<float>(rand() % 800);
        }
    }

    if (isChoosingCharacter)
    {
        for (int i = 0; i < 3; i++)
        {
            if (i == selectedCharacterIndex)
            {
                characterBoxes[i].setOutlineThickness(3.f);
                characterBoxes[i].setOutlineColor(sf::Color::Yellow);
            }
            else
            {
                characterBoxes[i].setOutlineThickness(0.f);
            }
        }
    }
    return StateAction::Keep;
}

void MenuState::render(sf::RenderWindow& window)
{
    window.setView(window.getDefaultView());

    // 1. RYSOWANIE GWIAZD W TLE
    for (const auto& star : stars)
    {
        sf::RectangleShape starShape(sf::Vector2f(star.size, star.size));
        starShape.setPosition(star.position);
        starShape.setFillColor(sf::Color(255, 255, 255, 180));
        window.draw(starShape);
    }

    // 2. RYSOWANIE ELEMENTÓW INTERFEJSU
    window.draw(title);
    window.draw(menuFrame);

    if (isChoosingCharacter)
    {
        for (int i = 0; i < 3; i++)
        {
            window.draw(characterBoxes[i]);
            window.draw(characterNames[i]);
        }
    }
    else
    {
        // ZMIANA: Pętla teraz rysuje 3 elementy (i < 3), a nie 2!
        for (int i = 0; i < 3; i++)
        {
            window.draw(menu[i]);
        }
    }
}