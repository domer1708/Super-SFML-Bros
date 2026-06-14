#include "State.h"
#include <cstdlib>

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

    std::string options[3] = { "Nowa Gra", "Wczytaj Gre", "Wyjdz" };
    for (int i = 0; i < 3; i++) {
        menu[i].setFont(font);
        menu[i].setFillColor(i == 0 ? sf::Color::Yellow : sf::Color::White);
        menu[i].setString(options[i]);
        menu[i].setCharacterSize(24);
        float wWidth = menu[i].getGlobalBounds().width;
        menu[i].setPosition(sf::Vector2f((800.f - wWidth) / 2.f, 250.f + (i * 60.f)));
    }

    selectedItemIndex = 0;
    initStars();
}

void MenuState::initStars()
{
    for (int i = 0; i < 60; i++)
    {
        Star star;
        star.position.x = static_cast<float>(rand() % 800);
        star.position.y = static_cast<float>(rand() % 600);
        star.speed = static_cast<float>((rand() % 150) + 50);
        star.size = static_cast<float>((rand() % 3) + 1);
        stars.push_back(star);
    }
}

StateAction MenuState::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if ((event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) && selectedItemIndex > 0)
        {
            menu[selectedItemIndex].setFillColor(sf::Color::White);
            selectedItemIndex--;
            menu[selectedItemIndex].setFillColor(sf::Color::Yellow);
        }
        else if ((event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) && selectedItemIndex < 2)
        {
            menu[selectedItemIndex].setFillColor(sf::Color::White);
            selectedItemIndex++;
            menu[selectedItemIndex].setFillColor(sf::Color::Yellow);
        }
        else if (event.key.code == sf::Keyboard::Enter)
        {
            if (selectedItemIndex == 0) return StateAction::Play; // Od razu Nowa Gra!
            else if (selectedItemIndex == 1) return StateAction::Load;
            else if (selectedItemIndex == 2) return StateAction::Exit;
        }
    }
    return StateAction::Keep;
}

StateAction MenuState::update(sf::Time dt)
{
    for (auto& star : stars)
    {
        star.position.y += star.speed * dt.asSeconds();
        if (star.position.y > 600.f)
        {
            star.position.y = 0.f;
            star.position.x = static_cast<float>(rand() % 800);
        }
    }
    return StateAction::Keep;
}

void MenuState::render(sf::RenderWindow& window)
{
    window.setView(window.getDefaultView());

    for (const auto& star : stars)
    {
        sf::RectangleShape starShape(sf::Vector2f(star.size, star.size));
        starShape.setPosition(star.position);
        starShape.setFillColor(sf::Color(255, 255, 255, 180));
        window.draw(starShape);
    }

    window.draw(title);
    window.draw(menuFrame);

    for (int i = 0; i < 3; i++)
    {
        window.draw(menu[i]);
    }
}