#include "State.h"
#include <cstdlib> // do losowania pozycji gwiazd

MenuState::MenuState()
{
    // Ładujemy nową, kanciatą czcionkę pixel art!
    // Jeśli nie pobrałeś jeszcze pliku, zmień tymczasowo na "pliki/arial.ttf"
    font.loadFromFile("pliki/PressStart2P-Regular.ttf"); 

    // --- TYTUŁ GRY (Wyśrodkowany i większy) ---
    title.setFont(font);                    
    title.setString("SUPER SFML BROS");
    title.setCharacterSize(42); // Dla czcionek pixelowych 42 jest ogromne i czytelne
    title.setFillColor(sf::Color::White);
    // Automatyczne centrowanie tytułu na osi X (ekran ma 800px)
    float titleWidth = title.getGlobalBounds().width;
    title.setPosition(sf::Vector2f((800.f - titleWidth) / 2.f, 60.f));
     
    // --- RAMKA MENU (Idealnie na środku) ---
    menuFrame.setSize(sf::Vector2f(340.f, 250.f));       
    menuFrame.setPosition(sf::Vector2f((800.f - 340.f) / 2.f, 200.f)); // X: 230, Y: 200
    menuFrame.setFillColor(sf::Color(20, 20, 20, 220)); // Ciemniejsze, ładniejsze tło
    menuFrame.setOutlineThickness(5.f);
    menuFrame.setOutlineColor(sf::Color::Yellow);

    // --- OPCJE MENU (Wyśrodkowane wewnątrz ramki) ---
    menu[0].setFont(font);                         
    menu[0].setFillColor(sf::Color::Yellow);
    menu[0].setString("Graj");
    menu[0].setCharacterSize(24);
    float gWidth = menu[0].getGlobalBounds().width;
    menu[0].setPosition(sf::Vector2f((800.f - gWidth) / 2.f, 250.f));

    menu[1].setFont(font);                         
    menu[1].setFillColor(sf::Color::White);
    menu[1].setString("Wyjdz");
    menu[1].setCharacterSize(24);
    float wWidth = menu[1].getGlobalBounds().width;
    menu[1].setPosition(sf::Vector2f((800.f - wWidth) / 2.f, 330.f));

    selectedItemIndex = 0;

    // --- INICJALIZACJA WYBORU POSTACI ---
    isChoosingCharacter = false;
    selectedCharacterIndex = 0;

    // Centrujemy też klocki wyboru postaci wewnątrz nowej ramki
    float startX = 265.f; 
    for (int i = 0; i < 3; i++) {
        characterBoxes[i].setSize(sf::Vector2f(65.f, 100.f));
        characterBoxes[i].setPosition(sf::Vector2f(startX + (i * 75.f), 240.f));
    }
    characterBoxes[0].setFillColor(sf::Color::Red);
    characterBoxes[1].setFillColor(sf::Color::Green);
    characterBoxes[2].setFillColor(sf::Color::Blue);

    characterNames[0].setString("Mario");
    characterNames[1].setString("Luigi");
    characterNames[2].setString("Toad");

    for (int i = 0; i < 3; i++) {
        characterNames[i].setFont(font);
        characterNames[i].setCharacterSize(14); // Mniejsza czcionka, żeby napisy nie błądziły
        characterNames[i].setFillColor(i == 0 ? sf::Color::Yellow : sf::Color::White);
        float nWidth = characterNames[i].getGlobalBounds().width;
        // Centrowanie napisu dokładnie pod jego własnym klockiem
        float boxCenter = characterBoxes[i].getPosition().x + 32.5f;
        characterNames[i].setPosition(sf::Vector2f(boxCenter - (nWidth / 2.f), 360.f));
    }

    // --- GENEROWANIE GWIAZD ---
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
            else if (event.key.code == sf::Keyboard::Escape)
            {
                isChoosingCharacter = false;
            }
            else if (event.key.code == sf::Keyboard::Enter)
            {
                return StateAction::Play; 
            }
        }
        else
        {
            if ((event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) && selectedItemIndex > 0)
            {
                menu[selectedItemIndex].setFillColor(sf::Color::White);
                selectedItemIndex--;
                menu[selectedItemIndex].setFillColor(sf::Color::Yellow);
            }
            else if ((event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) && selectedItemIndex < 1)
            {
                menu[selectedItemIndex].setFillColor(sf::Color::White);
                selectedItemIndex++;
                menu[selectedItemIndex].setFillColor(sf::Color::Yellow);
            }
            else if (event.key.code == sf::Keyboard::Enter)
            {
                if (selectedItemIndex == 0) 
                {
                    isChoosingCharacter = true; 
                }
                else if (selectedItemIndex == 1) 
                {
                    return StateAction::Exit;
                }
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

    // 1. RYSOWANIE GWIAZD W TLE (Najpierw, żeby były pod napisami)
    for (const auto& star : stars)
    {
        sf::RectangleShape starShape(sf::Vector2f(star.size, star.size));
        starShape.setPosition(star.position);
        starShape.setFillColor(sf::Color(255, 255, 255, 180)); // Lekko przezroczyste białe punkty
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
        for (int i = 0; i < 2; i++)
        {
            window.draw(menu[i]);
        }
    }
}