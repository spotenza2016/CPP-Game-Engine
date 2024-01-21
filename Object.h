#pragma once
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
using namespace std;

// Base Object
class Object {
protected:
    sf::Vector2f location;
    sf::Rect<float> collisionBox;
    vector<sf::Texture> textures;
    virtual void loadTextures() = 0;
    sf::Sprite sprite;
public:
    enum ACTION{WAIT = 0, MOVE = 1, REMOVE = 2, GENERATE = 3};
    virtual ACTION update(float time) = 0;
    bool collision = false;
    Object(sf::Vector2f location, float collisionWidth, float collisionHeight);
    sf::Vector2f getLocation();
    void setLocation(sf::Vector2f location);
    vector<sf::Texture> getTextures();
    sf::Sprite getSprite();
    void setSprite(sf::Sprite sprite);
    sf::Rect<float> getCollision();
    void setCollision(sf::Rect<float> collisionBox);
    virtual bool checkCollision(Object* object);

};

// Map Element
class MapElement : public Object {
protected:
    sf::Vector2f size;
public:
    sf::Vector2f getSize();
    void setSize(sf::Vector2f size);
    MapElement(sf::Vector2f location, float collisionWidth, float collisionHeight);

};

// Collision Element
class CollisionElement : public MapElement {
protected:
public:
    CollisionElement(sf::Vector2f location, float collisionWidth, float collisionHeight);
};

// Wall entity
class Wall : public CollisionElement {
protected:
    void loadTextures();
public:
    Wall(sf::Vector2f location);
    ACTION update(float time);
};

// Entity object
class Entity : public Object {
protected:
    float speedStat;
    sf::Vector2f direction;
public:
    Entity(sf::Vector2f location, float collisionWidth, float collisionHeight);
    float getSpeed();
    void setSpeed(float speed);
    sf::Vector2f getDirection();
    void setDirection(sf::Vector2f direction);

    void move(sf::Vector2f movement);
    virtual void collisionOccured(pair<bool, bool> collisionDirection);

};

// Bullet entity
class Bullet : public Entity {
protected:
    bool exists = true;
    void loadTextures();
    Object* parent;
public:
    Bullet(sf::Vector2f location, sf::Vector2f direction, Object* parent);
    void collisionOccured(pair<bool, bool> collisionDirection);
    bool checkCollision(Object* object);
    ACTION update(float time);
};

// Tower entity
class Tower : public Entity {
protected:
    float frameCounter = 0.0f;
    float frameDelay = 30.0f * (1.0f / 60.0f);
    void loadTextures();
    Bullet* generated = nullptr;
    float angleRad = 0.0f;
public:
    void shoot();
    Tower(sf::Vector2f location);
    void collisionOccured(pair<bool, bool> collisionDirection);
    ACTION update(float time);
    Bullet* getGenerated();
};

// Ball entity
class Ball : public Entity {
protected:
    void loadTextures();
public:
    Ball(sf::Vector2f location, sf::Vector2f direction);
    void collisionOccured(pair<bool, bool> collisionDirection);
    ACTION update(float time);
};

// Enemy entity (not yet implemented)
class Enemy : public Entity {
    Enemy();
};

// Player entity
class Player : public Entity {
protected:
    void loadTextures();
public:
    Player(sf::Vector2f location);
    ACTION update(float time);
    bool checkCollision(Object* object);
};
