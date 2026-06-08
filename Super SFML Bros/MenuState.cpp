

#include "State.h"

MenuState::MenuState()
{
    font.loadFromFile("pliki/arial.ttf"); // ładuje czcionkę

    title.setFont(font);                    // tytuł gry
    title.setString("SUPER SFML BROS");
    title.setCharacterSize(55);
    title.setFillColor(sf::Color::White);
    title.setPosition(sf::Vector2f(130.f, 70.f));

    menuFrame.setSize(sf::Vector2f(300.f, 250.f));       // ramka
    menuFrame.setPosition(sf::Vector2f(250.f, 180.f));
    menuFrame.setFillColor(sf::Color(30, 30, 30, 200));
    menuFrame.setOutlineThickness(5.f);
    menuFrame.setOutlineColor(sf::Color::Yellow);

    menu[0].setFont(font);                         // opcja gry
    menu[0].setFillColor(sf::Color::Yellow);
    menu[0].setString("Graj");
    menu[0].setPosition(sf::Vector2f(350.f, 230.f));

    menu[1].setFont(font);                         // opcja wyjścia
    menu[1].setFillColor(sf::Color::White);
    menu[1].setString("Wyjdz");
    menu[1].setPosition(sf::Vector2f(350.f, 320.f));

    selectedItemIndex = 0;

    // --- INICJALIZACJA WYBORU POSTACI (3 POSTACIE) ---
    isChoosingCharacter = false;
    selectedCharacterIndex = 0;

    // Postać 0: Mario (Czerwony)
    characterBoxes[0].setSize(sf::Vector2f(60.f, 100.f));
    characterBoxes[0].setFillColor(sf::Color::Red);
    characterBoxes[0].setPosition(sf::Vector2f(275.f, 220.f));

    characterNames[0].setFont(font);
    characterNames[0].setString("Mario");
    characterNames[0].setCharacterSize(20);
    characterNames[0].setFillColor(sf::Color::Yellow); // Domyślnie zaznaczony
    characterNames[0].setPosition(sf::Vector2f(280.f, 340.f));

    // Postać 1: Luigi (Zielony)
    characterBoxes[1].setSize(sf::Vector2f(60.f, 100.f));
    characterBoxes[1].setFillColor(sf::Color::Green);
    characterBoxes[1].setPosition(sf::Vector2f(370.f, 220.f));

    characterNames[1].setFont(font);
    characterNames[1].setString("Luigi");
    characterNames[1].setCharacterSize(20);
    characterNames[1].setFillColor(sf::Color::White);
    characterNames[1].setPosition(sf::Vector2f(375.f, 340.f));

    // Postać 2: Toad (Niebieski)
    characterBoxes[2].setSize(sf::Vector2f(60.f, 100.f));
    characterBoxes[2].setFillColor(sf::Color::Blue);
    characterBoxes[2].setPosition(sf::Vector2f(465.f, 220.f));

    characterNames[2].setFont(font);
    characterNames[2].setString("Toad");
    characterNames[2].setCharacterSize(20);
    characterNames[2].setFillColor(sf::Color::White);
    characterNames[2].setPosition(sf::Vector2f(475.f, 340.f));
}

StateAction MenuState::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        // 1. OBSŁUGA EKRANU WYBORU POSTACI
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
            else if (event.key.code == sf::Keyboard::Escape) // Powrót do menu głównego
            {
                isChoosingCharacter = false;
            }
            else if (event.key.code == sf::Keyboard::Enter) // Start gry
            {
                // W następnym etapie prześlemy stąd informację o wybranej postaci dalej!
                return StateAction::Play; 
            }
        }
        // 2. OBSŁUGA STANDARDOWEGO MENU GŁÓWNEGO
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
