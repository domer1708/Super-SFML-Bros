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

Trap::Trap(float x, float y, const sf::Texture& tex)
{
    sprite.setTexture(tex);

    // Dopasowanie rozmiaru kolca do klocka 50x50
    float scaleX = 50.f / tex.getSize().x;
    float scaleY = 50.f / tex.getSize().y;
    sprite.setScale(scaleX, scaleY);
    sprite.setPosition(x, y);

    // Hitbox (żebyś nie obrywał od pustego powietrza na krawędziach)
    // Zmniejszamy pole kolizji tylko do samego środka i dołu kolca
    hitbox.setSize(sf::Vector2f(20.f, 30.f));
    hitbox.setPosition(x + 15.f, y + 20.f);
    hitbox.setFillColor(sf::Color::Transparent); // Hitbox jest niewidzialny

    damage = 1;
}

void Trap::render(sf::RenderWindow& window)
{
    window.draw(sprite); // Rysujemy grafikę
}

sf::FloatRect Trap::getBounds() const
{
    return hitbox.getGlobalBounds(); // Gra sprawdza kolizję z węższym hitboxem!
}

int Trap::getDamage() const
{
    return damage;
}

Portal::Portal(float x, float y, const sf::Texture& tex, bool isLocked) {
    // 1. ZWIĘKSZAMY ROZMIAR DRZWI (Teraz są 2x szersze!)
    shape.setSize(sf::Vector2f(100.f, 100.f));

    // 2. KORYGUJEMY POZYCJĘ
    // x - 25.f przesuwa drzwi lekko w lewo, żeby były idealnie wyśrodkowane
    // y - 50.f unosi je w górę, żeby dół drzwi stał równo na podłodze
    shape.setPosition(x - 25.f, y - 50.f);

    locked = isLocked;

    // Nakładamy Twoją nową teksturę (bez żadnego wycinania setTextureRect!)
    shape.setTexture(&tex);

    if (isLocked) {
        // Jeśli drzwi są zamknięte na klucz, lekko je przyciemniamy
        shape.setFillColor(sf::Color(150, 150, 150));
    }
    else {
        // Otwarte drzwi mają swoje naturalne kolory
        shape.setFillColor(sf::Color::White);
    }
}

void Portal::render(sf::RenderWindow& window) { window.draw(shape); }

sf::FloatRect Portal::getBounds() const { return shape.getGlobalBounds(); }

void Portal::unlock()
{
    locked = false;
    shape.setFillColor(sf::Color::White); // Zmienia kolor na czysty po otwarciu!
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
Key::Key(float x, float y, const sf::Texture& tex) {
    sprite.setTexture(tex);

    // POWIĘKSZANIE: Skalujemy klucz do wymiarów np. 48x48 pikseli
    // (prawie cały kafel 50x50, żeby był widoczny)
    float targetSize = 48.f;
    float scaleX = targetSize / tex.getSize().x;
    float scaleY = targetSize / tex.getSize().y;
    sprite.setScale(scaleX, scaleY);

    // KORYGUJEMY POZYCJĘ (mniejsze przesunięcie, żeby wyśrodkować duży klucz)
    sprite.setPosition(x + 1.f, y + 1.f);

    // POWIĘKSZANIE HITBOXA: Zmieniamy na 48x48, żeby pasował do grafiki
    hitbox.setSize(sf::Vector2f(targetSize, targetSize));
    hitbox.setPosition(x + 1.f, y + 1.f);
    hitbox.setFillColor(sf::Color::Transparent); // Hitbox niewidzialny

    collected = false;
}

void Key::render(sf::RenderWindow& window) { if (!collected) window.draw(sprite); }
sf::FloatRect Key::getBounds() const { return hitbox.getGlobalBounds(); }
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

// --- WINDA (PIONOWA PLATFORMA) ---
Elevator::Elevator(float x, float y) {
    shape.setSize(sf::Vector2f(100.f, 30.f));
    shape.setFillColor(sf::Color(139, 69, 19)); // Brązowy kolor drewna
    shape.setPosition(x, y);
    startPos = sf::Vector2f(x, y);
    timeAccumulator = 0.f;
    speed = 1.5f;     // Prędkość poruszania się
    range = 200.f;    // Zasięg góra/dół
}

void Elevator::update(sf::Time dt) {
    timeAccumulator += dt.asSeconds();
    sf::Vector2f oldPos = shape.getPosition();

    // Ruch w pionie (funkcja sinus)
    float newY = startPos.y + std::sin(timeAccumulator * speed) * range;
    shape.setPosition(startPos.x, newY);

    deltaMovement = shape.getPosition() - oldPos;
}

void Elevator::render(sf::RenderWindow& window) { window.draw(shape); }
sf::FloatRect Elevator::getBounds() const { return shape.getGlobalBounds(); }