#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class PauseMenu 
{
private:
    sf::RectangleShape dimScreen;
    sf::RectangleShape frame;
    sf::Text menuItems[4];
    int selectedIndex;
    sf::Font& font;

public:
    PauseMenu(sf::Font& f);
    void handleInput(sf::Keyboard::Key key);
    int getSelectedIndex() const;
    void render(sf::RenderWindow& window);
    void setItemText(int index, std::string text);
};