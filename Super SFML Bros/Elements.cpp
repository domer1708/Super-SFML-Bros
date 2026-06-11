#include "Elements.h"
#include <cmath>
Star::Star(float x, float y)
{
    shape.setPointCount(10); // Nasza gwiazda ma 10 wierzchołków

    float outerRadius = 15.f; // Długość ramienia
    float innerRadius = 6.f;  // Głębokość wcięcia między ramionami
    const float PI = 3.14159265f;

    // Pętla przelicza i układa 10 punktów na kole
    for (int i = 0; i < 10; i++)
    {
        // Kąt dla kolejnego punktu (co 36 stopni, czyli PI/5 radianów).
        // Odejmujemy PI/2, aby pierwsze ramię gwiazdy wskazywało idealnie w górę.
        float angle = i * (PI / 5.f) - (PI / 2.f);

        // Parzyste punkty (0, 2, 4...) to ramiona, nieparzyste (1, 3, 5...) to wcięcia
        float radius = (i % 2 == 0) ? outerRadius : innerRadius;

        // Trygonometria obliczająca pozycję wierzchołka (X, Y)
        float px = radius * std::cos(angle) + outerRadius;
        float py = radius * std::sin(angle) + outerRadius;

        shape.setPoint(i, sf::Vector2f(px, py));
    }

    shape.setFillColor(sf::Color::Yellow);

    // Ustawienie gwiazdki na mapie (lekko wyśrodkowane w klocku 50x50)
    shape.setPosition(x + 10.f, y + 10.f);

    collected = false;
}

void Star::render(sf::RenderWindow& window)
{
    if (!collected)
    {
        window.draw(shape);
    }
}

sf::FloatRect Star::getBounds() const
{
    return shape.getGlobalBounds();
}

void Star::collect()
{
    collected = true;
}

bool Star::isCollected() const
{
    return collected;
}

Trap::Trap(float x, float y)
{
    shape.setRadius(20.f);
    shape.setPointCount(3);
    shape.setFillColor(sf::Color(150, 150, 150));
    shape.setPosition(x + 5.f, y + 10.f);
    damage = 1;
}

void Trap::render(sf::RenderWindow& window)
{
    window.draw(shape);
}

sf::FloatRect Trap::getBounds() const
{
    return shape.getGlobalBounds();
}

int Trap::getDamage() const
{
    return damage;
}

Portal::Portal(float x, float y, bool isLocked) {
    shape.setSize(sf::Vector2f(50.f, 100.f));
    if (isLocked) shape.setFillColor(sf::Color(50, 50, 50)); // Szary (zablokowany)
    else shape.setFillColor(sf::Color::Cyan); // Błękitny (otwarty)
    shape.setPosition(x, y - 50.f);
    locked = isLocked;
}

void Portal::render(sf::RenderWindow& window) { window.draw(shape); }

sf::FloatRect Portal::getBounds() const { return shape.getGlobalBounds(); }

void Portal::unlock() 
{
    locked = false;
    shape.setFillColor(sf::Color::Cyan); // Zmienia kolor po otwarciu!
}
bool Portal::isLocked() const { return locked; }

Mushroom::Mushroom(float x, float y, const sf::Texture& tex)
{
    collected = false;
    sprite.setTexture(tex);
    float scaleX = 30.f / tex.getSize().x;
    float scaleY = 30.f / tex.getSize().y;
    sprite.setScale(scaleX, scaleY);

    // 3. Pozycja – zachowujemy Wasze oryginalne przesunięcie na kafelku
    sprite.setPosition(x + 10.f, y + 20.f); 
}

void Mushroom::render(sf::RenderWindow& window)
{
    if (!collected) 
    {
        window.draw(sprite); // Rysujemy sprajta zamiast starego shape
    }
}

sf::FloatRect Mushroom::getBounds() const 
{ 
    return sprite.getGlobalBounds(); // Zwraca granice obrazka
}

void Mushroom::collect() 
{ 
    collected = true; 
}

bool Mushroom::isCollected() const 
{ 
    return collected; 
}

// --- TRAMPOLINA ---
Trampoline::Trampoline(float x, float y) {
    shape.setSize(sf::Vector2f(50.f, 25.f)); // O połowę niższa
    shape.setFillColor(sf::Color(255, 140, 0)); // Pomarańczowa
    shape.setPosition(x, y + 25.f); // Leży płasko na ziemi
}
void Trampoline::render(sf::RenderWindow& window) { window.draw(shape); }
sf::FloatRect Trampoline::getBounds() const { return shape.getGlobalBounds(); }

// --- KLUCZ ---
Key::Key(float x, float y) {
    shape.setSize(sf::Vector2f(20.f, 10.f));
    shape.setFillColor(sf::Color(255, 215, 0)); // Złoty
    shape.setPosition(x + 15.f, y + 20.f);
    collected = false;
}
void Key::render(sf::RenderWindow& window) { if (!collected) window.draw(shape); }
sf::FloatRect Key::getBounds() const { return shape.getGlobalBounds(); }
void Key::collect() { collected = true; }
bool Key::isCollected() const { return collected; }

// --- CHECKPOINT ---
Checkpoint::Checkpoint(float x, float y) {
    shape.setSize(sf::Vector2f(20.f, 50.f));
    shape.setFillColor(sf::Color(100, 100, 100)); // Szary = nieaktywny
    shape.setPosition(x + 15.f, y);
    activated = false;
}
void Checkpoint::render(sf::RenderWindow& window) { window.draw(shape); }
sf::FloatRect Checkpoint::getBounds() const { return shape.getGlobalBounds(); }
void Checkpoint::activate() {
    activated = true;
    shape.setFillColor(sf::Color::Blue); // Niebieski = aktywny!
}

// --- ZANIKAJĄCA PLATFORMA ---
VanishingPlatform::VanishingPlatform(float x, float y) {
    shape.setSize(sf::Vector2f(50.f, 50.f));
    shape.setFillColor(sf::Color(135, 206, 235));
    shape.setPosition(x, y);// Jasnoniebieski
    state = Normal;
    timer = 0.f;
}
void VanishingPlatform::trigger() {
    if (state == Normal) {
        state = Triggered;
        timer = 0.6; // Zniknie po 0.6 sekundy od dotknięcia
        shape.setFillColor(sf::Color::Red); // Miga na czerwono przed zniknięciem
    }
}
void VanishingPlatform::update(sf::Time dt) {
    if (state == Triggered) {
        timer -= dt.asSeconds();
        if (timer <= 0.f) {
            state = Vanished;
            timer = 2.5f; // Zniknie na 2.5 sekundy
        }
    }
    else if (state == Vanished) {
        timer -= dt.asSeconds();
        if (timer <= 0.f) {
            state = Normal;
            shape.setFillColor(sf::Color(135, 206, 235)); // Wraca do normy
        }
    }
}
void VanishingPlatform::render(sf::RenderWindow& window) {
    if (state != Vanished) window.draw(shape);
}
sf::FloatRect VanishingPlatform::getBounds() const { return shape.getGlobalBounds(); }

// --- POCISK ---
Bullet::Bullet(float x, float y) {
    shape.setSize(sf::Vector2f(15.f, 10.f));
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(x, y + 20.f);
    speed = 250.f; // Szybkość pocisku
    alive = true;
}
void Bullet::update(sf::Time dt) {
    shape.move(-speed * dt.asSeconds(), 0.f); // Leci w lewo
    if (shape.getPosition().x < -100.f) alive = false; // Usuń poza ekranem
}
void Bullet::render(sf::RenderWindow& window) { window.draw(shape); }
sf::FloatRect Bullet::getBounds() const { return shape.getGlobalBounds(); }

// --- WIEŻYCZKA ---
Turret::Turret(float x, float y) {
    shape.setSize(sf::Vector2f(50.f, 50.f));
    shape.setFillColor(sf::Color(70, 70, 70)); // Ciemnoszary metal
    shape.setPosition(x, y);
    shootTimer = 0.f;
    shootInterval = 10.0f; // Strzał co 2 sekundy
}
bool Turret::updateAndCheckShoot(sf::Time dt) {
    shootTimer += dt.asSeconds();
    if (shootTimer >= shootInterval) {
        shootTimer = 0.f;
        return true;
    }
    return false;
}
void Turret::render(sf::RenderWindow& window) { window.draw(shape); }
sf::FloatRect Turret::getBounds() const { return shape.getGlobalBounds(); }

// --- RUCHOMA PLATFORMA ---
MovingPlatform::MovingPlatform(float x, float y) {
    shape.setSize(sf::Vector2f(100.f, 30.f)); // Dłuższa, ale niższa
    shape.setFillColor(sf::Color(218, 165, 32)); // Złotawy/Brązowy
    shape.setPosition(x, y);
    startPos = sf::Vector2f(x, y);
    timeAccumulator = 0.f;
    speed = 2.f;      // Częstotliwość ruchu
    range = 150.f;    // Zasięg ruchu (150px w lewo i w prawo)
}
void MovingPlatform::update(sf::Time dt) {
    timeAccumulator += dt.asSeconds();
    sf::Vector2f oldPos = shape.getPosition();

    // Ruch w poziomie za pomocą funkcji sinus
    float newX = startPos.x + std::sin(timeAccumulator * speed) * range;
    shape.setPosition(newX, startPos.y);

    // Obliczanie przesunięcia w tej konkretnej klatce
    deltaMovement = shape.getPosition() - oldPos;
}
void MovingPlatform::render(sf::RenderWindow& window) { window.draw(shape); }
sf::FloatRect MovingPlatform::getBounds() const { return shape.getGlobalBounds(); }