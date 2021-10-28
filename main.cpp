
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//
#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <random>
#include <ctime>
#include <algorithm>
#include "ResourcePath.hpp"
using namespace sf;

// Here is a small helper for you! Have a look.

int buzzyRotation = 0;

float motionTime = 0.0f;
int score = 0;
bool restartGame = false;
bool hasGameStarted = false;
bool beeActive = false;
bool displayBee = true;

bool isBuzzyActive = false;
float beeSpeed = 0.0f;
float buzzySpeed = 0.0f;
float power = 0.0f;

std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(-1500, 1500);
std::uniform_int_distribution<int> heightDist(150, 300);

void centerText(sf::Text &text);
void setOriginToCenter(sf::Sprite &sprite);
sf::Vector2f round(const sf::Vector2f vector);
bool CollisionChecker(sf::Sprite &buzzy, sf::Sprite &r2);
void resetBuzzy(sf::Sprite &buzzy);


int myrandom(int i) { return std::rand() % i; }

int main(int, char const **)
{
    // Create the main window

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML window", sf::Style::Default);

    std::srand(unsigned(std::time(0)));

    sf::Texture buzzyLifeTexture;
    buzzyLifeTexture.loadFromFile(resourcePath() + "buzzy_life.png");
    std::vector<sf::Sprite> buzzyLives(5, sf::Sprite(buzzyLifeTexture));

    //Insect
    sf::Sprite insect;
    sf::Texture insectTexture;
    insectTexture.loadFromFile(resourcePath() + "insect.png");
    insect.setTexture(insectTexture);
    setOriginToCenter(insect);
    insect.setPosition(1900, 150);

    //buzzy
    sf::Sprite buzzy;
    sf::Texture buzzyTexture;
    buzzyTexture.loadFromFile(resourcePath() + "smallbuzzy.png");
    buzzy.setTexture(buzzyTexture);
    buzzy.setPosition(0, 700);

    //first row
    sf::Sprite sheep;
    sf::Texture sheepTexture;
    sheepTexture.loadFromFile(resourcePath() + "sheep.png");
    sheep.setTexture(sheepTexture);
    setOriginToCenter(sheep);

    sf::Sprite bunny;
    sf::Texture bunnyTexture;
    bunnyTexture.loadFromFile(resourcePath() + "bunny.png");
    bunny.setTexture(bunnyTexture);
    setOriginToCenter(bunny);

    sf::Sprite dog;
    sf::Texture dogTexture;
    dogTexture.loadFromFile(resourcePath() + "dog.png");
    dog.setTexture(dogTexture);
    setOriginToCenter(dog);

    sf::Sprite tiger;
    sf::Texture tigerTexture;
    tigerTexture.loadFromFile(resourcePath() + "tiger.png");
    tiger.setTexture(tigerTexture);
    setOriginToCenter(tiger);

    sf::Sprite mouse;
    sf::Texture mouseTexture;
    mouseTexture.loadFromFile(resourcePath() + "mouse.png");
    mouse.setTexture(mouseTexture);
    setOriginToCenter(mouse);

    std::vector<sf::Sprite> firstRow{sheep, bunny, dog, tiger, mouse};

    //second row
    sf::Sprite unicorn;
    sf::Texture unicornTexture;
    unicornTexture.loadFromFile(resourcePath() + "angry_unicorn.png");
    unicorn.setTexture(unicornTexture);
    setOriginToCenter(unicorn);

    sf::Sprite frog;
    sf::Texture frogTexture;
    frogTexture.loadFromFile(resourcePath() + "frog.png");
    frog.setTexture(frogTexture);
    setOriginToCenter(frog);

    sf::Sprite gaBulldog;
    sf::Texture gaBulldogTexture;
    gaBulldogTexture.loadFromFile(resourcePath() + "Georgia_Bulldogs.png");
    gaBulldog.setTexture(gaBulldogTexture);
    setOriginToCenter(gaBulldog);

    sf::Sprite pig;
    sf::Texture pigTexture;
    pigTexture.loadFromFile(resourcePath() + "pig.png");
    pig.setTexture(pigTexture);
    //    setOriginToCenter(pig);

    sf::Sprite chicken;
    sf::Texture chickenTexture;
    chickenTexture.loadFromFile(resourcePath() + "chicken.png");
    chicken.setTexture(chickenTexture);
    setOriginToCenter(chicken);

    std::vector<sf::Sprite> sndRow{unicorn, frog, gaBulldog, pig, chicken};



//     Use of shuffle
        std::random_shuffle(firstRow.begin(), firstRow.end(), myrandom);
        std::random_shuffle(sndRow.begin(), sndRow.end(), myrandom);

    const auto PI = std::acos(-1);
    float gravity = -196.2;

    //Power Bar
    sf::RectangleShape powerBar;
    float powerBarStartWidth = 0;
    float powerBarHeight = 50;
    powerBar.setSize(Vector2f(powerBarStartWidth, powerBarHeight));
    powerBar.setFillColor(Color::Red);
    powerBar.setPosition(300, 980);

    sf::RectangleShape powerBarShape;
    powerBarShape.setSize(Vector2f(410, 50));
    powerBarShape.setFillColor(Color::Transparent);
    powerBarShape.setOutlineColor(Color::Black);
    powerBarShape.setOutlineThickness(10);
    powerBarShape.setPosition(300, 980);

    sf::Time gameTimeTotal;

    float powerBarWidthPerSecond = 10;

    // Is the bee currently moving?

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png"))
    {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Load a sprite to display
    sf::Texture texture;
    if (!texture.loadFromFile(resourcePath() + "background.png"))
    {
        return EXIT_FAILURE;
    }
    sf::Sprite sprite(texture);

    // Create a graphical text to display
    sf::Font font;
    if (!font.loadFromFile(resourcePath() + "KOMIKAP_.ttf"))
    {
        return EXIT_FAILURE;
    }

    sf::Text text("Lives ", font, 40);
    text.setFillColor(sf::Color::White);
    text.setPosition(50, 60);

    sf::Text powerText("Power ", font);
    powerText.setFillColor(sf::Color::White);
    centerText(powerText);
    powerText.setPosition(200, 1020);

    sf::Text title("Buzzy's Revenge", font, 90);
    title.setFillColor(sf::Color::Red);
    centerText(title);
    title.setPosition(sf::Vector2f((window.getSize().x / 2u) - 50, 350));

    //center text
    String header = "Press Enter to Restart \n";
    String middle = "Press Esc to exit \n";
    String middle2 = "Press Space to Powerup \n";
    String end = "Press up/down arrow to aim \n";

    sf::Text instructions(header + middle + middle2 + end, font, 50);
    instructions.setFillColor(sf::Color::White);
    centerText(instructions);
    instructions.setPosition(sf::Vector2f(window.getSize().x / 2u, 800));

    // Load a music to play
    sf::Music music;
    if (!music.openFromFile(resourcePath() + "nice_music.ogg"))
    {
        return EXIT_FAILURE;
    }

    // Play the music
    music.play();

    // Variables to control time itself
    sf::Clock clock;
    sf::Clock clock2;

    sf::Text scoreText("", font);
    scoreText.setPosition(1750, 60);

    sf::Text author("", font);
    author.setString("\t\tCreated by\n Ghaly Nicolas Jules");
    centerText(author);
    author.setPosition(sf::Vector2f(window.getSize().x / 2u, 900));

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            //
            switch (event.type)
            {
                // Close window: exit
            case sf::Event::Closed:
                window.close();
                break;
                // Escape pressed: exit
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
                else if (event.key.code == sf::Keyboard::Enter)
                {
                    hasGameStarted = true;
                    restartGame = false;
                    music.stop();
                }
                else if (event.key.code == sf::Keyboard::Space)
                {
                    if (power <= float(40))
                    {
                        sf::Time dt1 = clock.restart();
                        power += dt1.asMilliseconds();
                    }
                }
                if (event.key.code == sf::Keyboard::Up)
                {
                    buzzy.rotate(-10.f);
                    buzzyRotation = 360 - buzzy.getRotation();
                    std::cout << buzzyRotation << ": " << buzzy.getRotation() << std::endl;
                }
                if (event.key.code == sf::Keyboard::Down)
                {
                    buzzy.rotate(10.f);
                    buzzyRotation = 360 - buzzy.getRotation();
                    std::cout << buzzyRotation << ": " << buzzy.getRotation() << std::endl;
                }
                break;
            case sf::Event::KeyReleased:
                if (event.key.code == sf::Keyboard::Space)
                {
                    isBuzzyActive = true;
                }
                //
                break;

            default:
                break;
            }
        }

        if (buzzyLives.size()==0)
        {
            restartGame = true;
        }

        if (restartGame)
        {
            buzzy.setPosition(-10, 700);
            motionTime = 0;
            buzzyRotation = 0;
            isBuzzyActive = false;
            insect.setPosition(1900, 150);
            beeActive = false;
            buzzySpeed = 0;
            hasGameStarted = false;
            score = 0;
            firstRow.clear();
            sndRow.clear();
            buzzyLives.clear();
            firstRow.insert(firstRow.end(), {sheep, bunny, dog, tiger, mouse});
            sndRow.insert(sndRow.end(), {unicorn, frog, gaBulldog, pig, chicken});
            buzzyLives.insert(buzzyLives.end(), 5, sf::Sprite(buzzyLifeTexture));
            std::random_shuffle(firstRow.begin(), firstRow.end(), myrandom);
            std::random_shuffle(sndRow.begin(), sndRow.end(), myrandom);
        }

        if (hasGameStarted)
        { //moving bee code
            sf::Time dt = clock.restart();
            powerBar.setSize(Vector2f(powerBarWidthPerSecond * power, powerBarHeight));

            // Setup the bee
                if (!beeActive)
                {

                    // How fast is the bee
                    srand((int)time(0) * 10);
                    beeSpeed = (rand() % 200) + 200;

                    srand((int)time(0) * 10);
                    float height = (rand() % 50) + 150;
                    insect.setPosition(1900, height);
                    beeActive = true;
                }
                else
                // Move the bee
                {

                    float heightInsect = distribution(generator);

                    insect.setPosition(
                        insect.getPosition().x -
                            (beeSpeed * dt.asSeconds()),
                        insect.getPosition().y + (heightInsect * dt.asSeconds()));

                    // Has the bee reached the right hand edge of the screen?
                    if (insect.getPosition().x < 0)
                    {
                        // Set it up ready to be a whole new cloud next frame
                        beeActive = false;
                    }
                }

            sf::Time dt4 = clock2.restart();

            if (isBuzzyActive)
            // Move the bee
            {
                if (power < 10)
                {
                    power = 10;
                }
                buzzySpeed = 1920 * power / 40;

                motionTime += dt4.asSeconds();

                float vix = buzzySpeed * cos(buzzyRotation * PI / 180) * motionTime;

                float viy = buzzySpeed * sin(buzzyRotation * PI / 180);

                float vfy = viy * motionTime + 0.5 * gravity * motionTime * motionTime;
                float angle = atan2(vix, vfy);
                angle = (angle * PI / 180);
                buzzy.setPosition(vix, 700 - vfy);
                buzzy.rotate(angle);
                
                if (CollisionChecker(buzzy, insect))
                {
                    score += 75;
                    displayBee = false;
                    resetBuzzy(buzzy);
                }
                
                else if(firstRow.size()>0)
                {
                    for (auto woodAnim : firstRow)
                    {
                        if(CollisionChecker(buzzy, woodAnim))
                        {
                            if (woodAnim.getOrigin().x == tiger.getOrigin().x && woodAnim.getOrigin().y == tiger.getOrigin().y )
                            {
                                score += 25;
                                firstRow.clear();
                                resetBuzzy(buzzy);
                                break;
                            }
                            else
                            {
                                resetBuzzy(buzzy);
                                buzzyLives.pop_back();
                            }
                        }
                    }
                    
                }
                else if (sndRow.size()>0)
                {
//                    int i = 0;
                    for (int i=0; i<sndRow.size();++i)
                    {
                        if(CollisionChecker(buzzy, sndRow[i]))
                        {
                            if (sndRow[i].getOrigin().x == gaBulldog.getOrigin().x && sndRow[i].getOrigin().y == gaBulldog.getOrigin().y )
                            {
                                score += 25;
                                sndRow.clear();
                                resetBuzzy(buzzy);
                                if (buzzyLives.size()>0 )
                                {
                                    firstRow.insert(firstRow.end(), {sheep, bunny, dog, tiger, mouse});
                                    sndRow.insert(sndRow.end(), {unicorn, frog, gaBulldog, pig, chicken});
                                    resetBuzzy(buzzy);
                                    std::random_shuffle(firstRow.begin(), firstRow.end(), myrandom);
                                    std::random_shuffle(sndRow.begin(), sndRow.end(), myrandom);
                                }
                                break;
                            }
                            else if (sndRow[i].getOrigin().x == unicorn.getOrigin().x && sndRow[i].getOrigin().y == unicorn.getOrigin().y )
                            {
                                auto lifeSize =buzzyLives.size();
                                if (lifeSize<5)
                                {
                                    buzzyLives.push_back(buzzyLives[lifeSize-1]);
                                }
                                sndRow.erase(sndRow.begin()+i);
                                resetBuzzy(buzzy);
                                break;
                            }
                            else
                            {
                                resetBuzzy(buzzy);
                                buzzyLives.pop_back();
                            }
                        }
                    }
                }

                if (buzzy.getPosition().x >= 1920 || buzzy.getPosition().y >= 1200)
                {
                    // Set it up ready to be a whole new cloud next frame
                    buzzyLives.pop_back();
                    resetBuzzy(buzzy);
                }
            }
            
        }

        //end of insect moving section

        scoreText.setString("Score: " + std::to_string(score));

        // Clear screen
        window.clear();

        // Draw the sprite
        window.draw(sprite);

        // Draw the string
        window.draw(text);

        for (int i = 0; i < buzzyLives.size(); ++i)
        {
            buzzyLives[i].setPosition(200 + i * 65, 50);
            window.draw(buzzyLives[i]);
        }
        
        if (firstRow.size()>0) {
            for (int i = 0; i < firstRow.size(); ++i)
            {
                firstRow[i].setPosition(1500, 920 - i * 150);
                window.draw(firstRow[i]);
            }
        }

        
        for (int i = 0; i < sndRow.size(); ++i)
        {
            sndRow[i].setPosition(1700, 920 - i * 150);
            window.draw(sndRow[i]);
        }

        if (displayBee) {
            window.draw(insect);
        }
        
        window.draw(powerBarShape);
        window.draw(powerBar);
        window.draw(powerText);

        window.draw(scoreText);

        
        
        window.draw(buzzy);

        if (!hasGameStarted)
        {
            window.draw(title);
            window.draw(instructions);
            window.draw(author);
        }

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}

void centerText(sf::Text &text)
{
    auto center = Vector2f(text.getGlobalBounds().width / 2.f, text.getGlobalBounds().height);
    auto localBounds = center + sf::Vector2f(text.getLocalBounds().left, text.getLocalBounds().top);
    auto rounded = round(localBounds);
    text.setOrigin(rounded);
}

void setOriginToCenter(sf::Sprite &sprite)
{
    auto center = Vector2f(sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height / 2);
    auto localBounds = center + sf::Vector2f(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top);
    auto rounded = round(localBounds);
    sprite.setOrigin(rounded);
}

bool CollisionChecker(sf::Sprite &buzzy, sf::Sprite &r2)
{
    
    if (buzzy.getGlobalBounds().intersects(r2.getGlobalBounds()))
    {
        return true;
    }

    return false;
}

sf::Vector2f round(const sf::Vector2f vector)
{
    return sf::Vector2f{std::round(vector.x), std::round(vector.y)};
}

void resetBuzzy(sf::Sprite &buzzy)
{
    buzzy.setPosition(-10, 700);
    isBuzzyActive = false;
    motionTime = 0;
    buzzy.setRotation(0);
    buzzySpeed = 0;
    power = 0;
    buzzyRotation = 0;
}
