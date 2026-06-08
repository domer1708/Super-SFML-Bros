#include "PauseMenu.h"

PauseMenu::PauseMenu(sf::Font& f) : font(f), selectedIndex(0) 
{
    // Tło (ściemnienie całego ekranu)
    dimScreen.setSize(sf::Vector2f(800.f, 600.f));
    dimScreen.setFillColor(sf::Color(0, 0, 0, 180));

    // Ramka menu
    frame.setSize(sf::Vector2f(400.f, 350.f));
    frame.setPosition(200.f, 125.f); // Wyśrodkowanie na ekranie
    frame.setFillColor(sf::Color(30, 30, 30, 240));
    frame.setOutlineThickness(5.f);
    frame.setOutlineColor(sf::Color::Yellow);

    // Opcje menu
    std::string options[4] = { "Wznow Gre", "Zapisz Gre", "Wczytaj Gre", "Wyjdz do Menu" };
    for (int i = 0; i < 4; i++) {
        menuItems[i].setFont(font);
        menuItems[i].setString(options[i]);
        menuItems[i].setCharacterSize(30);
        menuItems[i].setFillColor(i == 0 ? sf::Color::Yellow : sf::Color::White);

        float width = menuItems[i].getGlobalBounds().width;
        menuItems[i].setPosition(400.f - width / 2.f, 160.f + (i * 70.f));
    }
}

void PauseMenu::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::W || key == sf::Keyboard::Up) {
        if (selectedIndex > 0) {
            menuItems[selectedIndex].setFillColor(sf::Color::White);
            selectedIndex--;
            menuItems[selectedIndex].setFillColor(sf::Color::Yellow);
        }
    }
    else if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
        if (selectedIndex < 3) {
            menuItems[selectedIndex].setFillColor(sf::Color::White);
            selectedIndex++;
            menuItems[selectedIndex].setFillColor(sf::Color::Yellow);
        }
    }
}

int PauseMenu::getSelectedIndex() const {
    return selectedIndex;
}

void PauseMenu::render(sf::RenderWindow& window) {
    window.draw(dimScreen);
    window.draw(frame); // Rysujemy ramkę
    for (auto& item : menuItems) {
        window.draw(item);
    }
}

void PauseMenu::setItemText(int index, std::string text) {
    menuItems[index].setString(text);
}