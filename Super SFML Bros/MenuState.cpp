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
}
StateAction MenuState::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if ((event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) && selectedItemIndex > 0) // ruch w górę
        {
            menu[selectedItemIndex].setFillColor(sf::Color::White);
            selectedItemIndex--;
            menu[selectedItemIndex].setFillColor(sf::Color::Yellow);
        }
        else if ((event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) && selectedItemIndex < 1) // ruch w dół
        {
            menu[selectedItemIndex].setFillColor(sf::Color::White);
            selectedItemIndex++;
            menu[selectedItemIndex].setFillColor(sf::Color::Yellow);
        }
        else if (event.key.code == sf::Keyboard::Enter) // zatwierdzenie 
        {
            if (selectedItemIndex == 0) return StateAction::Play;
            if (selectedItemIndex == 1) return StateAction::Exit;
        }
    }
    return StateAction::Keep;
}

StateAction MenuState::update(sf::Time dt)
{
    return StateAction::Keep;
}

void MenuState::render(sf::RenderWindow& window)
{
    window.setView(window.getDefaultView());

    window.draw(menuFrame);
    window.draw(title); 

    for (int i = 0; i < 2; i++)
    {
        window.draw(menu[i]);
    }
}