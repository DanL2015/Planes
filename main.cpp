#include<SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Collision.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
using namespace sf;

struct movingProjectile
{
    Vector2f velocity;
    Vector2f position;
};

struct enemyPlane
{
    float planeTheta;
    float planeVelocity;
    // Clock planeClock;
    Sprite planeSprite;
    bool frontBlocked;
    bool planeMovement;
};

Vector2i windowSize = Vector2i(800, 800);
RenderWindow window(VideoMode(windowSize.x, windowSize.y), "planes.");
Font fffforward;
Texture plane1onTexture;
Texture plane1offTexture;
Texture plane2onTexture;
Texture plane2offTexture;
Sprite plane1Sprite;
Sprite plane2Sprite;
float plane1Velocity;
float plane1Theta;
const float padding = 32;
float movementMultiplier = 0.7; //should be based on deltatime
int difficulty = 0;
int numPlanes = 1;
const int maxPlanes = 10;
const float projectileSpeed = 1.001;
vector<movingProjectile> projectiles;
const Time shootingDelay = milliseconds(100);
Clock plane1Clock;
Time t1;
Clock timeClock;
Clock plane2Clock;
bool frontBlocked = false;
vector<enemyPlane> planesN;
const float collisionSize = 32;

void init()
{
    fffforward.loadFromFile("assets/fffforward.ttf");
    plane1onTexture.loadFromFile("assets/plane1on.png");
    plane1offTexture.loadFromFile("assets/plane1off.png");
    plane2onTexture.loadFromFile("assets/plane2on.png");
    plane2offTexture.loadFromFile("assets/plane2off.png");
    plane1Sprite.setTexture(plane1offTexture);
    plane1Sprite.setOrigin(Vector2f(plane1Sprite.getLocalBounds().width / 2, plane1Sprite.getLocalBounds().height / 2));
    plane1Sprite.setPosition(Vector2f(windowSize.x / 4, windowSize.y / 2));
    plane1Sprite.setScale(Vector2f(2, 2));
    plane1Velocity = 0;
    plane1Theta = 0;
    plane1Clock.restart();
    timeClock.restart();
    srand(time(NULL));
}

int welcomeScreen()
{
    int highlighted = 0; //0 for changing number of enemies, 1 for changing difficulty of planes.
    const int numOptions = 2;
    //Start Button
    Text startButtonText;
    startButtonText.setFont(fffforward);
    startButtonText.setString("start.");
    startButtonText.setOrigin(Vector2f(startButtonText.getLocalBounds().width / 2, startButtonText.getLocalBounds().height / 2));
    startButtonText.setPosition(Vector2f((windowSize.x) / 2, (windowSize.y) / 2));
    RectangleShape startButtonBackground1;
    startButtonBackground1.setFillColor(Color::Cyan);
    startButtonBackground1.setSize(Vector2f(startButtonText.getLocalBounds().width + padding, startButtonText.getLocalBounds().height + padding));
    startButtonBackground1.setOrigin(Vector2f(startButtonBackground1.getSize().x / 2, (startButtonBackground1.getSize().y) / 2));
    startButtonBackground1.setPosition(Vector2f(windowSize.x / 2, (windowSize.y) / 2));
    RectangleShape startButtonBackground2;
    startButtonBackground2.setFillColor(Color::Magenta);
    startButtonBackground2.setSize(Vector2f(startButtonText.getLocalBounds().width + padding, startButtonText.getLocalBounds().height + padding));
    startButtonBackground2.setOrigin(Vector2f(startButtonBackground2.getSize().x / 2, (startButtonBackground2.getSize().y) / 2));
    startButtonBackground2.setPosition(Vector2f(windowSize.x / 2, (windowSize.y) / 2));

    //Quit Button
    Text quitButtonText;
    quitButtonText.setFont(fffforward);
    quitButtonText.setString("quit.");
    quitButtonText.setOrigin(Vector2f(quitButtonText.getLocalBounds().width / 2, quitButtonText.getLocalBounds().height / 2));
    quitButtonText.setPosition(Vector2f((3 * windowSize.x) / 4, (3 * windowSize.y) / 4));
    RectangleShape quitButtonBackground1;
    quitButtonBackground1.setFillColor(Color::Cyan);
    quitButtonBackground1.setSize(Vector2f(quitButtonText.getLocalBounds().width + padding, quitButtonText.getLocalBounds().height + padding));
    quitButtonBackground1.setOrigin(Vector2f(quitButtonBackground1.getSize().x / 2, (quitButtonBackground1.getSize().y) / 2));
    quitButtonBackground1.setPosition(Vector2f(3 * windowSize.x / 4, (3 * windowSize.y - 20) / 4));
    RectangleShape quitButtonBackground2;
    quitButtonBackground2.setFillColor(Color::Magenta);
    quitButtonBackground2.setSize(Vector2f(quitButtonText.getLocalBounds().width + padding, quitButtonText.getLocalBounds().height + padding));
    quitButtonBackground2.setOrigin(Vector2f(quitButtonBackground2.getSize().x / 2, (quitButtonBackground2.getSize().y) / 2));
    quitButtonBackground2.setPosition(Vector2f(3 * windowSize.x / 4, (3 * windowSize.y - 20) / 4));

    //Difficulty Slider
    Text difficultyText;
    difficultyText.setFont(fffforward);
    difficultyText.setString("difficulty: " + to_string(difficulty));
    difficultyText.setFillColor(Color::White);
    difficultyText.setOrigin(Vector2f(difficultyText.getLocalBounds().width / 2, difficultyText.getLocalBounds().height / 2));
    difficultyText.setPosition(Vector2f((windowSize.x) / 2, (3 * windowSize.y) / 4));

    //Number of Planes Slider
    Text numPlanesText;
    numPlanesText.setFont(fffforward);
    numPlanesText.setString("number of enemies: " + to_string(numPlanes));
    numPlanesText.setFillColor(Color::White);
    numPlanesText.setOrigin(Vector2f(numPlanesText.getLocalBounds().width / 2, numPlanesText.getLocalBounds().height / 2));
    numPlanesText.setPosition(Vector2f((windowSize.x) / 2, (3 * windowSize.y) / 4 - numPlanesText.getOrigin().y * 2 - padding));

    Text promotionText;
    promotionText.setFont(fffforward);
    promotionText.setString("made by daniel liu using c++ and sfml.");
    promotionText.setFillColor(Color::White);
    promotionText.setOrigin(Vector2f(promotionText.getLocalBounds().width / 2, promotionText.getLocalBounds().height / 2));
    promotionText.setPosition(Vector2f((windowSize.x) / 2, windowSize.y - promotionText.getLocalBounds().height - padding));

    //Title Text
    Text titleText;
    titleText.setFont(fffforward);
    titleText.setString("planes.");
    titleText.setCharacterSize(100);
    titleText.setFillColor(Color::White);
    titleText.setOrigin(Vector2f(titleText.getLocalBounds().width / 2, titleText.getLocalBounds().height / 2));
    titleText.setPosition(Vector2f(windowSize.x / 2, windowSize.y / 4));

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::W || event.key.code == Keyboard::Up)
                {
                    highlighted++;
                    highlighted %= numOptions;
                }
                else if (event.key.code == Keyboard::S || event.key.code == Keyboard::Down)
                {
                    highlighted--;
                    if (highlighted < 0)
                    {
                        highlighted = numOptions;
                    }
                }
                else if (event.key.code == Keyboard::D || event.key.code == Keyboard::Right)
                {
                    if (highlighted == 0)
                    {
                        numPlanes++;
                        if (numPlanes > maxPlanes)
                        {
                            numPlanes = 1;
                        }
                        numPlanesText.setString("number of enemies: " + to_string(numPlanes));
                    }
                    if (highlighted == 1)
                    {
                        difficulty++;
                        difficulty %= 6;
                        difficultyText.setString("difficulty: " + to_string(difficulty));
                    }
                }
                else if (event.key.code == Keyboard::A || event.key.code == Keyboard::Left)
                {
                    if (highlighted == 0)
                    {
                        numPlanes--;
                        if (numPlanes < 1)
                        {
                            numPlanes = maxPlanes;
                        }
                        numPlanesText.setString("number of enemies: " + to_string(numPlanes));
                    }
                    if (highlighted == 1)
                    {
                        difficulty--;
                        if (difficulty < 0)
                            difficulty = 5;
                        difficultyText.setString("difficulty: " + to_string(difficulty));
                    }
                }
            }
        }
        window.clear();
        if (abs(Mouse::getPosition(window).x - startButtonBackground1.getPosition().x) < startButtonBackground1.getOrigin().x && abs(Mouse::getPosition(window).y - startButtonBackground1.getPosition().y) < startButtonBackground1.getOrigin().y)
        {
            window.draw(startButtonBackground2);
            startButtonText.setFillColor(Color::White);
            window.draw(startButtonText);
            if (Mouse::isButtonPressed(Mouse::Left))
            {
                return 1;
            }
        }
        else
        {
            window.draw(startButtonBackground1);
            startButtonText.setFillColor(Color::Black);
            window.draw(startButtonText);
        }
        if (abs(Mouse::getPosition(window).x - quitButtonBackground1.getPosition().x) < quitButtonBackground1.getOrigin().x && abs(Mouse::getPosition(window).y - quitButtonBackground1.getPosition().y) < quitButtonBackground1.getOrigin().y)
        {
            window.draw(quitButtonBackground2);
            quitButtonText.setFillColor(Color::White);
            window.draw(quitButtonText);
            if (Mouse::isButtonPressed(Mouse::Left))
            {
                return 0;
            }
        }
        else
        {
            window.draw(quitButtonBackground1);
            quitButtonText.setFillColor(Color::Black);
            window.draw(quitButtonText);
        }
        if (highlighted == 0)
        {
            numPlanesText.setFillColor(Color::Magenta);
            difficultyText.setFillColor(Color::White);
        }
        if (highlighted == 1)
        {
            numPlanesText.setFillColor(Color::White);
            difficultyText.setFillColor(Color::Magenta);
        }
        window.draw(difficultyText);
        window.draw(numPlanesText);
        window.draw(promotionText);
        window.draw(titleText);
        window.display();
    }
}

void endScreen(int deadPlane)
{
    Text endScreenText;
    endScreenText.setFont(fffforward);
    endScreenText.setFillColor(Color::White);
    if (deadPlane == 1)
    {
        endScreenText.setString("you lose.");
    }
    if (deadPlane == 2)
    {
        endScreenText.setString("you win.");
    }
    endScreenText.setOrigin(Vector2f(endScreenText.getLocalBounds().width / 2, endScreenText.getLocalBounds().height / 2));
    endScreenText.setPosition(Vector2f(windowSize.x / 2, windowSize.y / 2));
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(endScreenText);
        window.display();
    }
}

float deltaTime()
{
    Time ans = timeClock.getElapsedTime();
    timeClock.restart();
    return ans.asMicroseconds() / 500;
}

int main()
{
    init();
    int out = welcomeScreen();
    if (out == 0)
    {
        return 0;
    }

    //Initialize Planes
    for (int i = 0; i < numPlanes; i++)
    {
        planesN.push_back(enemyPlane{
            0,
            0,
            Sprite(plane2offTexture),
            false,
            false});
        // planesN[i].planeClock.restart();
        planesN[i].planeSprite.setOrigin(Vector2f(planesN[i].planeSprite.getLocalBounds().width / 2, planesN[i].planeSprite.getLocalBounds().height / 2));
        int curPlaneX = 400 + rand() % 400;
        int curPlaneY = rand() % 800;
        planesN[i].planeSprite.setPosition(Vector2f(curPlaneX, curPlaneY));
        planesN[i].planeSprite.setScale(Vector2f(2, 2));
    }

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        //Game Logic
        movementMultiplier = deltaTime();
        bool player1Movement = false;
        if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up))
        {
            plane1Velocity -= 0.0003 * movementMultiplier;
            player1Movement = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left))
        {
            plane1Theta -= 0.2 * movementMultiplier;
            if (plane1Theta < 0)
            {
                plane1Theta = 359;
            }
            player1Movement = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down))
        {
            plane1Velocity += 0.0003 * movementMultiplier;
            player1Movement = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
        {
            plane1Theta += 0.2 * movementMultiplier;
            if (plane1Theta >= 360)
            {
                plane1Theta = 0;
            }
            player1Movement = true;
        }
        t1 = plane1Clock.getElapsedTime();
        if ((Keyboard::isKeyPressed(Keyboard::Space) || Mouse::isButtonPressed(Mouse::Left)) && t1 > shootingDelay)
        {
            //Fire a projectile, definitely need to check to see if this works
            Vector2f projectileVelocity = Vector2f((-1) * ((-1) * (plane1Velocity + projectileSpeed) * cos((plane1Theta - 90) * M_PI / 180)), ((-1) * (plane1Velocity + projectileSpeed) * sin((-1) * (plane1Theta - 90) * M_PI / 180)));
            Vector2f projectilePosition = Vector2f(plane1Sprite.getPosition().x + cos((plane1Theta - 90) * M_PI / 180) * 32, plane1Sprite.getPosition().y + sin((plane1Theta - 90) * M_PI / 180) * 32);
            projectiles.push_back(movingProjectile{projectileVelocity, projectilePosition});
            plane1Clock.restart();
        }
        if (player1Movement)
        {
            plane1Sprite.setTexture(plane1onTexture);
        }
        else
        {
            plane1Sprite.setTexture(plane1offTexture);
        }
        plane1Sprite.setRotation(plane1Theta);
        Vector2f plane1SpriteMovement = Vector2f((-1) * (plane1Velocity * cos((plane1Theta - 90) * M_PI / 180)), (plane1Velocity * sin((-1) * (plane1Theta - 90) * M_PI / 180)));
        if (plane1Sprite.getPosition().x + (-1) * (plane1Velocity * cos((plane1Theta - 90) * M_PI / 180)) >= windowSize.x || plane1Sprite.getPosition().x + (-1) * (plane1Velocity * cos((plane1Theta - 90) * M_PI / 180)) < 0)
        {
            plane1SpriteMovement.x = 0;
        }
        if (plane1Sprite.getPosition().y + (plane1Velocity * sin((-1) * (plane1Theta - 90) * M_PI / 180)) >= windowSize.y || plane1Sprite.getPosition().y + (plane1Velocity * sin((-1) * (plane1Theta - 90) * M_PI / 180)) < 0)
        {
            plane1SpriteMovement.y = 0;
        }
        plane1Sprite.move(plane1SpriteMovement);
        //Deceleration proportional to velocity
        plane1Velocity -= plane1Velocity * 0.001 * movementMultiplier;

        //Bot logic
        for (int i = 0; i < planesN.size(); i++)
        {
            planesN[i].planeMovement = false;
            switch (difficulty)
            {
            case (0):
            {
                //Doesn't move
                // int movementChance = rand() % 100;
                // if (movementChance <= 10 && frontBlocked == false)
                // {
                //     plane2Velocity -= 0.0001 * movementMultiplier * 0.2;
                //     player2Movement = true;
                // }
                // else if (movementChance <= 20)
                // {
                //     plane2Velocity += 0.0001 * movementMultiplier * 0.2;
                //     player2Movement = true;
                // }
                // else if (movementChance <= 30)
                // {
                //     plane2Theta -= 0.2 * movementMultiplier * 0.2;
                //     if (plane2Theta < 0)
                //     {
                //         plane2Theta = 359;
                //     }
                //     player2Movement = true;
                // }
                // else if (movementChance <= 60)
                // {
                //     plane2Theta += 0.2 * movementMultiplier * 0.2;
                //     if (plane2Theta >= 360)
                //     {
                //         plane2Theta = 0;
                //     }
                //     player2Movement = true;
                // }
                // int shootingChance = rand() % 100;
                // if (shootingChance == 1 && plane2Clock.getElapsedTime() > shootingDelay)
                // {
                //     plane2Clock.restart();
                //     Vector2f projectileVelocity = Vector2f((-1) * ((-1) * (plane2Velocity + projectileSpeed) * cos((plane2Theta - 90) * M_PI / 180)), ((-1) * (plane2Velocity + projectileSpeed) * sin((-1) * (plane2Theta - 90) * M_PI / 180)));
                //     Vector2f projectilePosition = Vector2f(plane2Sprite.getPosition().x + cos((plane2Theta - 90) * M_PI / 180) * 16, plane2Sprite.getPosition().y + sin((plane2Theta - 90) * M_PI / 180) * 16);
                //     projectiles.push_back(movingProjectile{projectileVelocity, projectilePosition});
                // }
                // if (player2Movement)
                // {
                //     plane2Sprite.setTexture(plane2onTexture);
                // }
                // else
                // {
                //     plane2Sprite.setTexture(plane2offTexture);
                // }

                // plane2Sprite.setRotation(plane2Theta);
                // Vector2f plane2SpriteMovement = Vector2f((-1) * (plane2Velocity * cos((plane2Theta - 90) * M_PI / 180)), (plane2Velocity * sin((-1) * (plane2Theta - 90) * M_PI / 180)));
                // if (plane2Sprite.getPosition().x + (-1) * (plane2Velocity * cos((plane2Theta - 90) * M_PI / 180)) >= windowSize.x || plane2Sprite.getPosition().x + (-1) * (plane2Velocity * cos((plane2Theta - 90) * M_PI / 180)) < 0)
                // {
                //     frontBlocked = true;
                //     plane2SpriteMovement.x = 0;
                // }
                // else
                //     frontBlocked = false;
                // if (plane2Sprite.getPosition().y + (plane2Velocity * sin((-1) * (plane2Theta - 90) * M_PI / 180)) >= windowSize.y || plane2Sprite.getPosition().y + (plane2Velocity * sin((-1) * (plane2Theta - 90) * M_PI / 180)) < 0)
                // {
                //     frontBlocked = true;
                //     plane2SpriteMovement.y = 0;
                // }
                // else
                //     frontBlocked = false;
                // plane2Sprite.move(plane2SpriteMovement);
                // plane2Velocity -= plane2Velocity * 0.001 * movementMultiplier;
                break;
            }
            case (1):
            {
                //pretty stupid bot, moves slightly faster
                int movementChance = rand() % 100;
                if (movementChance <= 10 && frontBlocked == false)
                {
                    planesN[i].planeVelocity -= 0.0001 * movementMultiplier * 0.1;
                    planesN[i].planeMovement = true;
                }
                else if (movementChance <= 20)
                {
                    planesN[i].planeVelocity += 0.0001 * movementMultiplier * 0.1;
                    planesN[i].planeMovement = true;
                }
                else if (movementChance <= 30)
                {
                    planesN[i].planeTheta -= 0.2 * movementMultiplier * 0.1;
                    if (planesN[i].planeTheta < 0)
                    {
                        planesN[i].planeTheta = 359;
                    }
                    planesN[i].planeMovement = true;
                }
                else if (movementChance <= 60)
                {
                    planesN[i].planeTheta += 0.2 * movementMultiplier * 0.1;
                    if (planesN[i].planeTheta >= 360)
                    {
                        planesN[i].planeTheta = 0;
                    }
                    planesN[i].planeMovement = true;
                }
                int shootingChance = rand() % 100;
                // if (shootingChance == 1 && planesN[i].planeClock.getElapsedTime() > shootingDelay)
                if (shootingChance == 1)
                {
                    // planesN[i].planeClock.restart();
                    Vector2f projectileVelocity = Vector2f((-1) * ((-1) * (planesN[i].planeVelocity + projectileSpeed) * cos((planesN[i].planeTheta - 90) * M_PI / 180)), ((-1) * (planesN[i].planeVelocity + projectileSpeed) * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)));
                    Vector2f projectilePosition = Vector2f(planesN[i].planeSprite.getPosition().x + cos((planesN[i].planeTheta - 90) * M_PI / 180) * 32, planesN[i].planeSprite.getPosition().y + sin((planesN[i].planeTheta - 90) * M_PI / 180) * 32);
                    projectiles.push_back(movingProjectile{projectileVelocity, projectilePosition});
                }
                if (planesN[i].planeMovement)
                {
                    planesN[i].planeSprite.setTexture(plane2onTexture);
                }
                else
                {
                    planesN[i].planeSprite.setTexture(plane2offTexture);
                }

                planesN[i].planeSprite.setRotation(planesN[i].planeTheta);
                Vector2f planeNSpriteMovement = Vector2f((-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)), (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)));
                if (planesN[i].planeSprite.getPosition().x + (-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)) >= windowSize.x || planesN[i].planeSprite.getPosition().x + (-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)) < 0)
                {
                    frontBlocked = true;
                    //cout << "You lost" << endl;
                    planeNSpriteMovement.x = 0;
                }
                else
                    frontBlocked = false;
                if (planesN[i].planeSprite.getPosition().y + (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)) >= windowSize.y || planesN[i].planeSprite.getPosition().y + (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)) < 0)
                {
                    frontBlocked = true;
                    planeNSpriteMovement.y = 0;
                }
                else
                    frontBlocked = false;
                planesN[i].planeSprite.move(planeNSpriteMovement);
                planesN[i].planeVelocity -= planesN[i].planeVelocity * 0.001 * movementMultiplier;
                break;
            }
            case (2):
            {
                //slightly faster
                int movementChance = rand() % 100;
                if (movementChance <= 10 && frontBlocked == false)
                {
                    planesN[i].planeVelocity -= 0.0001 * movementMultiplier;
                    planesN[i].planeMovement = true;
                }
                else if (movementChance <= 20)
                {
                    planesN[i].planeVelocity += 0.0001 * movementMultiplier;
                    planesN[i].planeMovement = true;
                }
                else if (movementChance <= 30)
                {
                    planesN[i].planeTheta -= 0.2 * movementMultiplier;
                    if (planesN[i].planeTheta < 0)
                    {
                        planesN[i].planeTheta = 359;
                    }
                    planesN[i].planeMovement = true;
                }
                else if (movementChance <= 60)
                {
                    planesN[i].planeTheta += 0.2 * movementMultiplier;
                    if (planesN[i].planeTheta >= 360)
                    {
                        planesN[i].planeTheta = 0;
                    }
                    planesN[i].planeMovement = true;
                }
                int shootingChance = rand() % 100;
                // if (shootingChance == 1 && planesN[i].planeClock.getElapsedTime() > shootingDelay)
                if (shootingChance == 1)
                {
                    // planesN[i].planeClock.restart();
                    Vector2f projectileVelocity = Vector2f((-1) * ((-1) * (planesN[i].planeVelocity + projectileSpeed) * cos((planesN[i].planeTheta - 90) * M_PI / 180)), ((-1) * (planesN[i].planeVelocity + projectileSpeed) * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)));
                    Vector2f projectilePosition = Vector2f(planesN[i].planeSprite.getPosition().x + cos((planesN[i].planeTheta - 90) * M_PI / 180) * 32, planesN[i].planeSprite.getPosition().y + sin((planesN[i].planeTheta - 90) * M_PI / 180) * 32);
                    projectiles.push_back(movingProjectile{projectileVelocity, projectilePosition});
                }
                if (planesN[i].planeMovement)
                {
                    planesN[i].planeSprite.setTexture(plane2onTexture);
                }
                else
                {
                    planesN[i].planeSprite.setTexture(plane2offTexture);
                }

                planesN[i].planeSprite.setRotation(planesN[i].planeTheta);
                //Check both
                Vector2f planeNSpriteMovement = Vector2f((-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)), (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)));
                if (planesN[i].planeSprite.getPosition().x + (-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)) >= windowSize.x || planesN[i].planeSprite.getPosition().x + (-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)) < 0)
                {
                    frontBlocked = true;
                    planeNSpriteMovement.x = 0;
                }
                else
                    frontBlocked = false;
                if (planesN[i].planeSprite.getPosition().y + (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)) >= windowSize.y || planesN[i].planeSprite.getPosition().y + (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)) < 0)
                {
                    frontBlocked = true;
                    planeNSpriteMovement.y = 0;
                }
                else
                    frontBlocked = false;
                planesN[i].planeSprite.move(planeNSpriteMovement);
                planesN[i].planeVelocity -= planesN[i].planeVelocity * 0.001 * movementMultiplier;
                break;
            }
            case (3):
            {
                Vector2f deltaPos = Vector2f(planesN[i].planeSprite.getPosition().x - plane1Sprite.getPosition().x, planesN[i].planeSprite.getPosition().y - plane1Sprite.getPosition().y);
                float slopeTheta = atan(deltaPos.y / deltaPos.x) * 180 / M_PI;
                if (deltaPos.x < 0)
                {
                    slopeTheta = 180 + slopeTheta;
                }
                // slopeTheta = 270 - slopeTheta;
                if (slopeTheta < 0)
                {
                    slopeTheta += 359;
                }
                if (slopeTheta >= 360)
                {
                    slopeTheta -= 360;
                }
                float deltaTheta = planesN[i].planeTheta - slopeTheta;

                if (deltaTheta < 0)
                {
                    deltaTheta += 360;
                }
                if (deltaTheta >= 360)
                {
                    deltaTheta -= 360;
                }

                if (deltaTheta > 90 && deltaTheta < 270)
                {
                    // decrease theta

                    planesN[i].planeTheta += 0.2 * movementMultiplier * 0.15;
                    if (planesN[i].planeTheta >= 360)
                    {
                        planesN[i].planeTheta = 0;
                    }
                    planesN[i].planeMovement = true;
                }
                else
                {
                    //increase theta

                    planesN[i].planeTheta -= 0.2 * movementMultiplier * 0.15;
                    if (planesN[i].planeTheta < 0)
                    {
                        planesN[i].planeTheta = 359;
                    }
                    planesN[i].planeMovement = true;
                }

                int movementChance = rand() % 100;
                if (movementChance <= 5)
                {
                    planesN[i].planeVelocity -= 0.002 * movementMultiplier;
                    planesN[i].planeMovement = true;
                }

                int shootingChance = rand() % 1000;
                // if (shootingChance == 1 && plane2Clock.getElapsedTime() > shootingDelay)
                if (shootingChance == 1)
                {
                    // planesN[i].planeClock.restart();
                    Vector2f projectileVelocity = Vector2f((-1) * ((-1) * (planesN[i].planeVelocity + projectileSpeed) * cos((planesN[i].planeTheta - 90) * M_PI / 180)), ((-1) * (planesN[i].planeVelocity + projectileSpeed) * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)));
                    Vector2f projectilePosition = Vector2f(planesN[i].planeSprite.getPosition().x + cos((planesN[i].planeTheta - 90) * M_PI / 180) * 32, planesN[i].planeSprite.getPosition().y + sin((planesN[i].planeTheta - 90) * M_PI / 180) * 32);
                    projectiles.push_back(movingProjectile{projectileVelocity, projectilePosition});
                }
                if (planesN[i].planeMovement)
                {
                    planesN[i].planeSprite.setTexture(plane2onTexture);
                }
                else
                {
                    planesN[i].planeSprite.setTexture(plane2offTexture);
                }

                planesN[i].planeSprite.setRotation(planesN[i].planeTheta);
                //Check both
                Vector2f planeNSpriteMovement = Vector2f((-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)), (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)));
                if (planesN[i].planeSprite.getPosition().x + (-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)) >= windowSize.x || planesN[i].planeSprite.getPosition().x + (-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)) < 0)
                {
                    planeNSpriteMovement.x = 0;
                }
                if (planesN[i].planeSprite.getPosition().y + (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)) >= windowSize.y || planesN[i].planeSprite.getPosition().y + (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)) < 0)
                {
                    planeNSpriteMovement.y = 0;
                }
                planesN[i].planeSprite.move(planeNSpriteMovement);
                planesN[i].planeVelocity -= planesN[i].planeVelocity * 0.001 * movementMultiplier;
                break;
            }
            case (4):
            {
                Vector2f deltaPos = Vector2f(planesN[i].planeSprite.getPosition().x - plane1Sprite.getPosition().x, planesN[i].planeSprite.getPosition().y - plane1Sprite.getPosition().y);
                float slopeTheta = atan(deltaPos.y / deltaPos.x) * 180 / M_PI;
                if (deltaPos.x < 0)
                {
                    slopeTheta = 180 + slopeTheta;
                }
                // slopeTheta = 270 - slopeTheta;
                if (slopeTheta < 0)
                {
                    slopeTheta += 359;
                }
                if (slopeTheta >= 360)
                {
                    slopeTheta -= 360;
                }
                float deltaTheta = planesN[i].planeTheta - slopeTheta;

                if (deltaTheta < 0)
                {
                    deltaTheta += 360;
                }
                if (deltaTheta >= 360)
                {
                    deltaTheta -= 360;
                }

                if (deltaTheta > 90 && deltaTheta < 270)
                {
                    // decrease theta

                    planesN[i].planeTheta += 0.2 * movementMultiplier;
                    if (planesN[i].planeTheta >= 360)
                    {
                        planesN[i].planeTheta = 0;
                    }
                    planesN[i].planeMovement = true;
                }
                else
                {
                    //increase theta

                    planesN[i].planeTheta -= 0.2 * movementMultiplier * 0.3;
                    if (planesN[i].planeTheta < 0)
                    {
                        planesN[i].planeTheta = 359;
                    }
                    planesN[i].planeMovement = true;
                }

                int movementChance = rand() % 100;
                if (movementChance <= 10)
                {
                    planesN[i].planeVelocity -= 0.002 * movementMultiplier * 0.3;
                    planesN[i].planeMovement = true;
                }

                int shootingChance = rand() % 1000;
                // if (shootingChance <= 5 && planesN[i].planeClock.getElapsedTime() > shootingDelay)
                if (shootingChance <= 5)
                {
                    // planesN[i].planeClock.restart();
                    Vector2f projectileVelocity = Vector2f((-1) * ((-1) * (planesN[i].planeVelocity + projectileSpeed) * cos((planesN[i].planeTheta - 90) * M_PI / 180)), ((-1) * (planesN[i].planeVelocity + projectileSpeed) * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)));
                    Vector2f projectilePosition = Vector2f(planesN[i].planeSprite.getPosition().x + cos((planesN[i].planeTheta - 90) * M_PI / 180) * 32, planesN[i].planeSprite.getPosition().y + sin((planesN[i].planeTheta - 90) * M_PI / 180) * 32);
                    projectiles.push_back(movingProjectile{projectileVelocity, projectilePosition});
                }
                if (planesN[i].planeMovement)
                {
                    planesN[i].planeSprite.setTexture(plane2onTexture);
                }
                else
                {
                    planesN[i].planeSprite.setTexture(plane2offTexture);
                }

                planesN[i].planeSprite.setRotation(planesN[i].planeTheta);
                //Check both
                Vector2f planeNSpriteMovement = Vector2f((-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)), (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)));
                if (planesN[i].planeSprite.getPosition().x + (-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)) >= windowSize.x || planesN[i].planeSprite.getPosition().x + (-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)) < 0)
                {
                    planeNSpriteMovement.x = 0;
                }
                if (planesN[i].planeSprite.getPosition().y + (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)) >= windowSize.y || planesN[i].planeSprite.getPosition().y + (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)) < 0)
                {
                    planeNSpriteMovement.y = 0;
                }
                planesN[i].planeSprite.move(planeNSpriteMovement);
                planesN[i].planeVelocity -= planesN[i].planeVelocity * 0.001 * movementMultiplier;
                break;
            }
            case (5):
            {
                Vector2f deltaPos = Vector2f(planesN[i].planeSprite.getPosition().x - plane1Sprite.getPosition().x, planesN[i].planeSprite.getPosition().y - plane1Sprite.getPosition().y);
                float slopeTheta = atan(deltaPos.y / deltaPos.x) * 180 / M_PI;
                if (deltaPos.x < 0)
                {
                    slopeTheta = 180 + slopeTheta;
                }
                // slopeTheta = 270 - slopeTheta;
                if (slopeTheta < 0)
                {
                    slopeTheta += 359;
                }
                if (slopeTheta >= 360)
                {
                    slopeTheta -= 360;
                }
                float deltaTheta = planesN[i].planeTheta - slopeTheta;

                if (deltaTheta < 0)
                {
                    deltaTheta += 360;
                }
                if (deltaTheta >= 360)
                {
                    deltaTheta -= 360;
                }

                if (deltaTheta > 90 && deltaTheta < 270)
                {
                    // decrease theta

                    planesN[i].planeTheta += 0.2 * movementMultiplier * 0.5;
                    if (planesN[i].planeTheta >= 360)
                    {
                        planesN[i].planeTheta = 0;
                    }
                    planesN[i].planeMovement = true;
                }
                else
                {
                    //increase theta

                    planesN[i].planeTheta -= 0.2 * movementMultiplier * 0.5;
                    if (planesN[i].planeTheta < 0)
                    {
                        planesN[i].planeTheta = 359;
                    }
                    planesN[i].planeMovement = true;
                }

                // cout << deltaTheta << endl;

                int movementChance = rand() % 100;
                if (movementChance <= 20)
                {
                    planesN[i].planeVelocity -= 0.002 * movementMultiplier;
                    planesN[i].planeMovement = true;
                }

                int shootingChance = rand() % 1000;
                // if (shootingChance <= 50 && planesN[i].planeClock.getElapsedTime() > shootingDelay)
                if (shootingChance <= 50)
                {
                    // planesN[i].planeClock.restart();
                    Vector2f projectileVelocity = Vector2f((-1) * ((-1) * (planesN[i].planeVelocity + projectileSpeed) * cos((planesN[i].planeTheta - 90) * M_PI / 180)), ((-1) * (planesN[i].planeVelocity + projectileSpeed) * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)));
                    Vector2f projectilePosition = Vector2f(planesN[i].planeSprite.getPosition().x + cos((planesN[i].planeTheta - 90) * M_PI / 180) * 32, planesN[i].planeSprite.getPosition().y + sin((planesN[i].planeTheta - 90) * M_PI / 180) * 32);
                    projectiles.push_back(movingProjectile{projectileVelocity, projectilePosition});
                }
                if (planesN[i].planeMovement)
                {
                    planesN[i].planeSprite.setTexture(plane2onTexture);
                }
                else
                {
                    planesN[i].planeSprite.setTexture(plane2offTexture);
                }

                planesN[i].planeSprite.setRotation(planesN[i].planeTheta);
                //Check both
                Vector2f planeNSpriteMovement = Vector2f((-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)), (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)));
                if (planesN[i].planeSprite.getPosition().x + (-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)) >= windowSize.x || planesN[i].planeSprite.getPosition().x + (-1) * (planesN[i].planeVelocity * cos((planesN[i].planeTheta - 90) * M_PI / 180)) < 0)
                {
                    planeNSpriteMovement.x = 0;
                }
                if (planesN[i].planeSprite.getPosition().y + (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)) >= windowSize.y || planesN[i].planeSprite.getPosition().y + (planesN[i].planeVelocity * sin((-1) * (planesN[i].planeTheta - 90) * M_PI / 180)) < 0)
                {
                    planeNSpriteMovement.y = 0;
                }
                planesN[i].planeSprite.move(planeNSpriteMovement);
                planesN[i].planeVelocity -= planesN[i].planeVelocity * 0.001 * movementMultiplier;
                break;
            }
            }
        }

        //Check projectiles
        for (int i = 0; i < projectiles.size(); i++)
        {
            projectiles[i].position.x += projectiles[i].velocity.x * movementMultiplier;
            projectiles[i].position.y += projectiles[i].velocity.y * movementMultiplier;
            Texture projectileTexture;
            projectileTexture.loadFromFile("assets/projectile.png");
            Sprite projectileSprite;
            projectileSprite.setTexture(projectileTexture);
            projectileSprite.setPosition(projectiles[i].position);
            bool erased = false;
            for (int j = 0; j < planesN.size(); j++)
            {
                // if (abs(projectiles[i].position.x - planesN[i].planeSprite.getPosition().x) < collisionSize && abs(projectiles[i].position.y - planesN[i].planeSprite.getPosition().y) < collisionSize)
                if (Collision::PixelPerfectTest(planesN[j].planeSprite, projectileSprite))
                {
                    projectiles.erase(projectiles.begin() + i);
                    erased = true;
                    planesN.erase(planesN.begin() + j);
                    if (j >= 0)
                        j--;
                    if (planesN.size() == 0)
                    {
                        endScreen(2);
                        return 0;
                    }
                }
            }
            if (erased)
            {
                i--;
            }
            else if (Collision::PixelPerfectTest(plane1Sprite, projectileSprite))
            {
                projectiles.erase(projectiles.begin() + i);
                i--;
                endScreen(1);
                return 0;
            }
            else if (projectiles[i].position.x > windowSize.x || projectiles[i].position.x <= 0 || projectiles[i].position.y > windowSize.y || projectiles[i].position.y <= 0)
            {
                projectiles.erase(projectiles.begin() + i);
                i--;
            }
        }

        //Display
        window.clear();
        for (int i = 0; i < projectiles.size(); i++)
        {
            RectangleShape projectileShape;
            projectileShape.setFillColor(Color::Green);
            projectileShape.setPosition(projectiles[i].position);
            projectileShape.setSize(Vector2f(4, 4));
            window.draw(projectileShape);
        }
        for (int i = 0; i < planesN.size(); i++)
        {
            window.draw(planesN[i].planeSprite);
        }
        window.draw(plane1Sprite);
        window.display();
    }

    return 0;
}
