#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

int main() 
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Mario");
    window.setFramerateLimit(144);
    sf::RectangleShape pleyer(sf::Vector2f(50,25));
    pleyer.setFillColor(sf::Color::Blue);
   
    
    float pozycja_mario_x = 200;
    float pozycja_mario_y = 300;


    float predkosc_x = 0;
    float predkosc_y = 0;

    const float szybkosc_ruchu = 700;
    const float grawitacja = 1800;
    const float sila_skoku = -700;
    bool czy_na_ziemi = false;
    sf::RectangleShape podloga(sf::Vector2f(1200, 100));
    podloga.setFillColor(sf::Color(100, 250, 100));
    podloga.setPosition(40, 600);

    sf::Clock clock;
    while (window.isOpen()) 
    {
        float delta_czas = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        
        predkosc_x = 0;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            predkosc_x = szybkosc_ruchu;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            predkosc_x = -szybkosc_ruchu;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            predkosc_y = sila_skoku;
        }

        predkosc_y += grawitacja * delta_czas;

        pozycja_mario_x += predkosc_x * delta_czas;
        pozycja_mario_y += predkosc_y * delta_czas;

        pleyer.setPosition(pozycja_mario_x, pozycja_mario_y);
        
        sf::FloatRect granice_gracza = pleyer.getGlobalBounds();
        sf::FloatRect granice_podlogi = podloga.getGlobalBounds();
        if(granice_gracza.intersects(granice_podlogi))
        {
            pozycja_mario_y = podloga.getPosition().y - pleyer.getSize().y;

            predkosc_y = 0;

            czy_na_ziemi = true;

            pleyer.setPosition(pozycja_mario_x, pozycja_mario_y);
        }
        else
        {
            czy_na_ziemi = false;
        }


        window.clear(sf::Color::Black);
        
        window.draw(pleyer);
        window.draw(podloga);
        window.display();
    }
    
    return 0;
}